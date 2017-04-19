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

#ifndef PCAP_DEALER_H_
#define PCAP_DEALER_H_

#include <unistd.h>
#include <pcap.h>
#include <vector>
#include <string.h>
using namespace std;

struct eap_header
{
    uint8_t eapol_version;
    uint8_t eapol_type; // 0x01 - Start, 0x02 - Logoff, 0x00 - EAP Packet
    uint16_t eapol_length; // equal to eap_length
    uint8_t eap_code;
    uint8_t eap_id;
    uint16_t eap_length;
    uint8_t eap_type;
    uint8_t eap_md5_value_size;
    uint8_t eap_md5_value[16];
};

class pcap_dealer
{
public:
	pcap_dealer(string device, vector<uint8_t> mac); // EAP/EAPOL
	pcap_dealer(string device, uint16_t port); // UDP
	bool init(string device, char filter[]);
	bool send(vector<uint8_t> data, vector<uint8_t> *success, string *error);
	void send_without_response(vector<uint8_t> data, string *error);
	bool recv(vector<uint8_t> *success, string *error);
    static bool testNICAccessibility(string device);
	virtual ~pcap_dealer();

private:
	pcap_t *handle;
	
};

#endif
