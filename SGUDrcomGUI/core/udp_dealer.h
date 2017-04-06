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

#ifndef UDP_DEALER_H_
#define UDP_DEALER_H_

#include "def.h"
#include "socket_dealer.h"
#include <time.h>
using namespace std;

class udp_dealer
{
public:
	udp_dealer(vector<uint8_t> local_mac, string local_ip, string dst_ip, uint16_t port);

	bool send_u8_pkt();
	bool send_u244_pkt(string login_username, string hostname, string local_dns_1, string local_dns_2);
	bool sendalive_u40_1_pkt();
	bool sendalive_u40_2_pkt();
	bool sendalive_u38_pkt(vector<uint8_t> md5_challenge_value);
	bool sendalive_u40_3_pkt();
	
	void generate_244_chksum(vector<uint8_t> &data_buf);
	void generate_40_chksum(vector<uint8_t> &data_buf);
	bool u244_retrieved_u8(vector<uint8_t> &udp_packet_u8resp);
	bool u40_retrieved_last(vector<uint8_t> &udp_packet_last);
	bool u38_retrieved_u244resp(vector<uint8_t> &udp_packet_u244resp);

	uint8_t udp_id_counter();
	void clear_udp_param();
	virtual ~udp_dealer();
	
private:
	socket_dealer sock;
    uint16_t port_to;
	uint8_t udp_pkt_id;

	vector<uint8_t> local_mac;
	string local_ip;
	string dst_ip;

	uint32_t random_byte;
	vector<uint8_t> u244_checksum;
	vector<uint8_t> u244_retrieved_byte; // u8 response packet(8-11bit)
	vector<uint8_t> u40_retrieved_byte;
	uint8_t u38_reserved_byte[3]; // calculated by the retrieved byte from u244 response packet(25-26 and 31 bit)
};

#endif
