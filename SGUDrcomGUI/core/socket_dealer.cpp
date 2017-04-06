/**
 * Copyright (C) 2017 Edward & Steven
 *
 * Licensed under the GPL, Version 3.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 *
 *               <http://fsf.org/>
 *
 * Everyone is permitted to copy and distribute verbatim copies of this license document.
 * Changing it is not allowed.
 *
 */

#include "socket_dealer.h"
#include "sgudrcom_exception.h"
#if defined(__APPLE__) || defined(__MACH__)
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL SO_NOSIGPIPE
#endif
#endif

socket_dealer::socket_dealer(string gateway_ip, uint16_t gateway_port, string local_ip) {
	client_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_fd < 0)
		throw sgudrcom_exception("socket", errno);

	int flag = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flag | O_NONBLOCK);

	struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = 0; // system defined
    local.sin_addr.s_addr = inet_addr(local_ip.c_str());
    bind(client_fd, (struct sockaddr *)&local, sizeof(local));

    gateway.sin_family = AF_INET;
    gateway.sin_port = htons(gateway_port);
    gateway.sin_addr.s_addr = inet_addr(gateway_ip.c_str());
}

bool socket_dealer::send_udp_pkt(vector<uint8_t> &udp_data_set, vector<uint8_t> &recv, string &error) {
	try
	{
		int total = 0;
		int left = (int) udp_data_set.size();
		while (total < udp_data_set.size())
		{
			int len = (int) sendto(client_fd, &udp_data_set[0], udp_data_set.size(), MSG_NOSIGNAL, (struct sockaddr *)&gateway, sizeof(gateway));
			if (len < 0)
			{
				if (errno == EWOULDBLOCK && left > 0)
					continue;
				else
					throw sgudrcom_exception("sendto", errno);
			}
			total += len;
            left -= len;
		}

		int ret = wait_for_socket();
        if (ret < 0) throw sgudrcom_exception("select", errno);
        if (ret == 0) throw sgudrcom_exception("select: timeout");

        struct sockaddr_in src_addr;
        while (true)
        {
            vector<uint8_t> buf(RECV_BUFF_LEN, 0);
            size_t addr_len = sizeof(src_addr);
            int len = 0;
            while (buf[0] == 0x4d || buf[0] == 0x00) // Notification
            {
                len = (int) recvfrom(client_fd, (char *)&buf[0], RECV_BUFF_LEN, 0, (struct sockaddr *)&src_addr, (socklen_t *)&addr_len);
            }
            
            if (len <= 0)
                break; // connection closed
            
            buf.resize(len);
            recv.insert(recv.end(), buf.begin(), buf.end());
            
            if (len < RECV_BUFF_LEN)
                break;
        }
	}
	catch(exception &e)
	{
		error = e.what();
		return false;
	}
	return true;
}

int socket_dealer::wait_for_socket(int timeout_milisec)
{
    fd_set fds;
    struct timeval tv;
    
    FD_ZERO(&fds);
    FD_SET(client_fd, &fds);
    
    tv.tv_usec = 0;
    tv.tv_sec = timeout_milisec / 1000;
    
    return select(client_fd + 1, &fds, NULL, NULL, &tv);
}

socket_dealer::~socket_dealer() {
	close(client_fd); // close socket
}
