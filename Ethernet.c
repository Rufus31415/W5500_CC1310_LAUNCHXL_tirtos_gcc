/*
 modified 12 Aug 2013
 by Soohwan Kim (suhwan@wiznet.co.kr)
*/
#include "Ethernet.h"
#include "Dhcp.h"

EthernetConfiguration Ethernet;

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
//uint8_t Ethernet_state[MAX_SOCK_NUM] = { 0, };
uint16_t Ethernet_server_port[MAX_SOCK_NUM] = { 0, };



int Ethernet_begin_mac(uint8_t *mac_address)
{
  DHCP dhcp;

  Ethernet._dhcp = &dhcp;

  // Initialise the basic info
  W5500_init();
  W5500_setMACAddress(mac_address);
  IPAddress ip; // = { 0,0,0,0 };
  ip.bytes[0] = 0;
  ip.bytes[1] = 0;
  ip.bytes[2] = 0;
  ip.bytes[3] = 0;

  W5500_setIPAddress(ip.bytes);

  // Now try to get our config info from a DHCP server
  int ret =  DHCP_beginWithDHCP(Ethernet._dhcp,mac_address);
  if(ret == 1)
  {
    // We've successfully found a DHCP server and got our configuration info, so set things
    // accordingly
    W5500_setIPAddress(DHCP_getLocalIp(Ethernet._dhcp).bytes);
    W5500_setGatewayIp(DHCP_getGatewayIp(Ethernet._dhcp).bytes);
    W5500_setSubnetMask(DHCP_getSubnetMask(Ethernet._dhcp).bytes);
	Ethernet._dnsServerAddress.dword = DHCP_getDnsServerIp(Ethernet._dhcp).dword;
  }

  return ret;
}

void Ethernet_begin_mac_ip(uint8_t *mac_address, IPAddress local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server.bytes[3] = 1;
  Ethernet_begin_mac_ip_dns(mac_address, local_ip, dns_server);
}

void Ethernet_begin_mac_ip_dns(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway.bytes[3] = 1;
  Ethernet_begin_mac_ip_dns_gateway(mac_address, local_ip, dns_server, gateway);
}

void Ethernet_begin_mac_ip_dns_gateway(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
	IPAddress subnet;
    subnet.bytes[0] = 255;
    subnet.bytes[1] = 255;
    subnet.bytes[2] = 255;
    subnet.bytes[3] = 0;

  Ethernet_begin_mac_ip_dns_gateway_subnet(mac_address, local_ip, dns_server, gateway, subnet);
}

void Ethernet_begin_mac_ip_dns_gateway_subnet(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
  W5500_init();
  W5500_setMACAddress(mac);
  W5500_setIPAddress(local_ip.bytes);
  W5500_setGatewayIp(gateway.bytes);
  W5500_setSubnetMask(subnet.bytes);
  Ethernet._dnsServerAddress = dns_server;
}

int Ethernet_maintain(){
  int rc = DHCP_CHECK_NONE;
  if(Ethernet._dhcp != NULL){
    //we have a pointer to dhcp, use it
    rc = DHCP_checkLease(Ethernet._dhcp);
    switch ( rc ){
      case DHCP_CHECK_NONE:
        //nothing done
        break;
      case DHCP_CHECK_RENEW_OK:
      case DHCP_CHECK_REBIND_OK:
        //we might have got a new IP.
        W5500_setIPAddress(DHCP_getLocalIp(Ethernet._dhcp).bytes);
        W5500_setGatewayIp(DHCP_getGatewayIp(Ethernet._dhcp).bytes);
        W5500_setSubnetMask(DHCP_getSubnetMask(Ethernet._dhcp).bytes);
		Ethernet._dnsServerAddress.dword = DHCP_getDnsServerIp(Ethernet._dhcp).dword;
        break;
      default:
        //this is actually a error, it will retry though
        break;
    }
  }
  return rc;
}

IPAddress Ethernet_localIP()
{
  IPAddress ret;
  W5500_getIPAddress(ret.bytes);
  return ret;
}

IPAddress Ethernet_subnetMask()
{
  IPAddress ret;
  W5500_getSubnetMask(ret.bytes);
  return ret;
}

IPAddress Ethernet_gatewayIP()
{
  IPAddress ret;
  W5500_getGatewayIp(ret.bytes);
  return ret;
}

IPAddress Ethernet_dnsServerIP()
{
  return Ethernet._dnsServerAddress;
}
