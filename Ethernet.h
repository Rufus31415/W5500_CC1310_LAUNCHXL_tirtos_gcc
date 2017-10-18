/*
 modified 12 Aug 2013
 by Soohwan Kim (suhwan@wiznet.co.kr)
*/
#ifndef Ethernet_h
#define Ethernet_h

#include <inttypes.h>
#include "W5500.h"
#include "IPAddress.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "Dhcp.h"

#ifdef __cplusplus
extern "C" {
#endif



	typedef struct Ethernet_ {
		IPAddress _dnsServerAddress;
		DHCP* _dhcp;
		uint8_t _state[MAX_SOCK_NUM];
		uint16_t _server_port[MAX_SOCK_NUM];
	}EthernetConfiguration;

	// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
	//extern uint8_t Ethernet_state[MAX_SOCK_NUM];

	extern uint16_t Ethernet_server_port[MAX_SOCK_NUM];


	extern EthernetConfiguration Ethernet;

	// Initialize the Ethernet shield to use the provided MAC address and gain the rest of the
	// configuration through DHCP.
	// Returns 0 if the DHCP configuration failed, and 1 if it succeeded
	int Ethernet_begin_mac(uint8_t *mac_address);
	void Ethernet_begin_mac_ip(uint8_t *mac_address, IPAddress local_ip);
	void Ethernet_begin_mac_ip_dns(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server);
	void Ethernet_begin_mac_ip_dns_gateway(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
	void Ethernet_begin_mac_ip_dns_gateway_subnet(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);


	int Ethernet_maintain();

	IPAddress Ethernet_localIP();
	IPAddress Ethernet_subnetMask();
	IPAddress Ethernet_gatewayIP();
	IPAddress Ethernet_dnsServerIP();

#ifdef __cplusplus
}
#endif

#endif
