// DHCP Library v0.3 - April 25, 2009
// Author: Jordan Terrell - blog.jordanterrell.com

#include "W5500.h"

#include <string.h>
#include <stdlib.h>
#include "Dhcp.h"
#ifdef ARDUINO
#include "Arduino.h"
#else
#include "ArduinoCompatibility.h"
#endif
#include "util.h"

int DHCP_beginWithDHCP(DHCP* dhcp, uint8_t *mac)
{
	dhcp->_dhcpLeaseTime=0;
	dhcp->_dhcpT1=0;
	dhcp->_dhcpT2=0;
	dhcp->_lastCheck=0;
	dhcp->_timeout = 60000;
	dhcp->_responseTimeout = 4000;

	EthernetUDP_begin_init(&dhcp->_dhcpUdpSocket);

    // zero out _dhcpMacAddr
    memset(dhcp->_dhcpMacAddr, 0, 6);
	DHCP_reset_DHCP_lease(dhcp);

    memcpy((void*)dhcp->_dhcpMacAddr, (void*)mac, 6);
	dhcp->_dhcp_state = STATE_DHCP_START;
    return DHCP_request_DHCP_lease(dhcp);
}

void DHCP_reset_DHCP_lease(DHCP* dhcp){
    // zero out _dhcpSubnetMask, _dhcpGatewayIp, _dhcpLocalIp, _dhcpDhcpServerIp, _dhcpDnsServerIp
    memset(dhcp->_dhcpLocalIp, 0, 20);
}

//return:0 on error, 1 if request is sent and response is received
int DHCP_request_DHCP_lease(DHCP* dhcp){
    
    uint8_t messageType = 0;
  
    
  
    // Pick an initial transaction ID
	dhcp->_dhcpTransactionId = 1+ random()*(2000-1)/ 0x7FFFFFFF;
	dhcp->_dhcpInitialTransactionId = dhcp->_dhcpTransactionId;

	EthernetUDP_stop(&dhcp->_dhcpUdpSocket);
    if (EthernetUDP_begin(&dhcp->_dhcpUdpSocket, DHCP_CLIENT_PORT) == 0)
    {
      // Couldn't get a socket
      return 0;
    }
    
    DHCP_presend_DHCP(dhcp);
    
    int result = 0;
    
    unsigned long startTime = millis();
    
    while(dhcp->_dhcp_state != STATE_DHCP_LEASED)
    {
        if(dhcp->_dhcp_state == STATE_DHCP_START)
        {
			dhcp->_dhcpTransactionId++;
            
            DHCP_send_DHCP_MESSAGE(dhcp, DHCP_DISCOVER, ((millis() - startTime) / 1000));
			dhcp->_dhcp_state = STATE_DHCP_DISCOVER;
        }
        else if(dhcp->_dhcp_state == STATE_DHCP_REREQUEST){
			dhcp->_dhcpTransactionId++;
            DHCP_send_DHCP_MESSAGE(dhcp,DHCP_REQUEST, ((millis() - startTime)/1000));
			dhcp->_dhcp_state = STATE_DHCP_REQUEST;
        }
        else if(dhcp->_dhcp_state == STATE_DHCP_DISCOVER)
        {
            uint32_t respId;
            messageType = DHCP_parseDHCPResponse(dhcp,dhcp->_responseTimeout, &respId);
            if(messageType == DHCP_OFFER)
            {
                // We'll use the transaction ID that the offer came with,
                // rather than the one we were up to
				dhcp->_dhcpTransactionId = respId;
                DHCP_send_DHCP_MESSAGE(dhcp, DHCP_REQUEST, ((millis() - startTime) / 1000));
				dhcp->_dhcp_state = STATE_DHCP_REQUEST;
            }
        }
        else if(dhcp->_dhcp_state == STATE_DHCP_REQUEST)
        {
            uint32_t respId;
            messageType = DHCP_parseDHCPResponse(dhcp, dhcp->_responseTimeout, &respId);
            if(messageType == DHCP_ACK)
            {
				dhcp->_dhcp_state = STATE_DHCP_LEASED;
                result = 1;
                //use default lease time if we didn't get it
                if(dhcp->_dhcpLeaseTime == 0){
					dhcp->_dhcpLeaseTime = DEFAULT_LEASE;
                }
                //calculate T1 & T2 if we didn't get it
                if(dhcp->_dhcpT1 == 0){
                    //T1 should be 50% of _dhcpLeaseTime
					dhcp->_dhcpT1 = dhcp->_dhcpLeaseTime >> 1;
                }
                if(dhcp->_dhcpT2 == 0){
                    //T2 should be 87.5% (7/8ths) of _dhcpLeaseTime
					dhcp->_dhcpT2 = dhcp->_dhcpT1 << 1;
                }
				dhcp->_renewInSec = dhcp->_dhcpT1;
				dhcp->_rebindInSec = dhcp->_dhcpT2;
            }
            else if(messageType == DHCP_NAK)
				dhcp->_dhcp_state = STATE_DHCP_START;
        }
        
        if(messageType == 255)
        {
            messageType = 0;
			dhcp->_dhcp_state = STATE_DHCP_START;
        }
        
        if(result != 1 && ((millis() - startTime) >dhcp->_timeout))
            break;
    }
    
    // We're done with the socket now
	EthernetUDP_stop(&dhcp->_dhcpUdpSocket);
	dhcp->_dhcpTransactionId++;

    return result;
}

void DHCP_presend_DHCP(DHCP* dhcp)
{
}

void DHCP_send_DHCP_MESSAGE(DHCP* dhcp, uint8_t messageType, uint16_t secondsElapsed)
{
    uint8_t buffer[32];
    memset(buffer, 0, 32);
	IPAddress dest_addr; // = { 255, 255, 255, 255 }; // Broadcast address
    dest_addr.bytes[0] = 255;
    dest_addr.bytes[1] = 255;
    dest_addr.bytes[2] = 255;
    dest_addr.bytes[3] = 255;

    if (-1 == EthernetUDP_beginPacket_ip(&dhcp->_dhcpUdpSocket,dest_addr, DHCP_SERVER_PORT))
    {
        // FIXME Need to return errors
        return;
    }

    buffer[0] = DHCP_BOOTREQUEST;   // op
    buffer[1] = DHCP_HTYPE10MB;     // htype
    buffer[2] = DHCP_HLENETHERNET;  // hlen
    buffer[3] = DHCP_HOPS;          // hops

    // xid
    unsigned long xid = htonl(dhcp->_dhcpTransactionId);
    memcpy(buffer + 4, &(xid), 4);

    // 8, 9 - seconds elapsed
    buffer[8] = ((secondsElapsed & 0xff00) >> 8);
    buffer[9] = (secondsElapsed & 0x00ff);

    // flags
    unsigned short flags = htons(DHCP_FLAGSBROADCAST);
    memcpy(buffer + 10, &(flags), 2);

    // ciaddr: already zeroed
    // yiaddr: already zeroed
    // siaddr: already zeroed
    // giaddr: already zeroed

    //put data in W5500 transmit buffer
	EthernetUDP_write(&dhcp->_dhcpUdpSocket,buffer, 28);

    memset(buffer, 0, 32); // clear local buffer

    memcpy(buffer, dhcp->_dhcpMacAddr, 6); // chaddr

    //put data in W5500 transmit buffer
	EthernetUDP_write(&dhcp->_dhcpUdpSocket,buffer, 16);

    memset(buffer, 0, 32); // clear local buffer

    // leave zeroed out for sname && file
    // put in W5500 transmit buffer x 6 (192 bytes)
  
    for(int i = 0; i < 6; i++) {
		EthernetUDP_write(&dhcp->_dhcpUdpSocket,buffer, 32);
    }
  
    // OPT - Magic Cookie
    buffer[0] = (uint8_t)((MAGIC_COOKIE >> 24)& 0xFF);
    buffer[1] = (uint8_t)((MAGIC_COOKIE >> 16)& 0xFF);
    buffer[2] = (uint8_t)((MAGIC_COOKIE >> 8)& 0xFF);
    buffer[3] = (uint8_t)(MAGIC_COOKIE& 0xFF);

    // OPT - message type
    buffer[4] = dhcpMessageType;
    buffer[5] = 0x01;
    buffer[6] = messageType; //DHCP_REQUEST;

    // OPT - client identifier
    buffer[7] = dhcpClientIdentifier;
    buffer[8] = 0x07;
    buffer[9] = 0x01;
    memcpy(buffer + 10, dhcp->_dhcpMacAddr, 6);

    // OPT - host name
    buffer[16] = hostName;
    buffer[17] = strlen(HOST_NAME) + 6; // length of hostname + last 3 bytes of mac address
    strcpy((char*)&(buffer[18]), HOST_NAME);

    DHCP_printByte((char*)&(buffer[24]), dhcp->_dhcpMacAddr[3]);
	DHCP_printByte((char*)&(buffer[26]), dhcp->_dhcpMacAddr[4]);
	DHCP_printByte((char*)&(buffer[28]), dhcp->_dhcpMacAddr[5]);

    //put data in W5500 transmit buffer
	EthernetUDP_write(&dhcp->_dhcpUdpSocket,buffer, 30);

    if(messageType == DHCP_REQUEST)
    {
        buffer[0] = dhcpRequestedIPaddr;
        buffer[1] = 0x04;
        buffer[2] = dhcp->_dhcpLocalIp[0];
        buffer[3] = dhcp->_dhcpLocalIp[1];
        buffer[4] = dhcp->_dhcpLocalIp[2];
        buffer[5] = dhcp->_dhcpLocalIp[3];

        buffer[6] = dhcpServerIdentifier;
        buffer[7] = 0x04;
        buffer[8] = dhcp->_dhcpDhcpServerIp[0];
        buffer[9] = dhcp->_dhcpDhcpServerIp[1];
        buffer[10] = dhcp->_dhcpDhcpServerIp[2];
        buffer[11] = dhcp->_dhcpDhcpServerIp[3];

        //put data in W5500 transmit buffer
		EthernetUDP_write(&dhcp->_dhcpUdpSocket,buffer, 12);
    }
    
    buffer[0] = dhcpParamRequest;
    buffer[1] = 0x06;
    buffer[2] = subnetMask;
    buffer[3] = routersOnSubnet;
    buffer[4] = dns;
    buffer[5] = domainName;
    buffer[6] = dhcpT1value;
    buffer[7] = dhcpT2value;
    buffer[8] = endOption;
    
    //put data in W5500 transmit buffer
	EthernetUDP_write(&dhcp->_dhcpUdpSocket,buffer, 9);

	EthernetUDP_endPacket(&dhcp->_dhcpUdpSocket);
}

uint8_t DHCP_parseDHCPResponse(DHCP* dhcp,unsigned long responseTimeout, uint32_t* transactionId)
{
    uint8_t type = 0;
    uint8_t opt_len = 0;
     
    unsigned long startTime = millis();

    while(EthernetUDP_parsePacket(&dhcp->_dhcpUdpSocket) <= 0)
    {
        if((millis() - startTime) > responseTimeout)
        {
            return 255;
        }
        delay(50);
    }
    // start reading in the packet
    RIP_MSG_FIXED fixedMsg;
	EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, (uint8_t*)&fixedMsg, sizeof(RIP_MSG_FIXED));
  
    if(fixedMsg.op == DHCP_BOOTREPLY && EthernetUDP_remotePort(&dhcp->_dhcpUdpSocket) == DHCP_SERVER_PORT)
    {
        uint32_t _transactionId = ntohl(fixedMsg.xid);
		*transactionId = _transactionId;
        if(memcmp(fixedMsg.chaddr, dhcp->_dhcpMacAddr, 6) != 0 || (_transactionId < dhcp->_dhcpInitialTransactionId) || (_transactionId > dhcp->_dhcpTransactionId))
        {
            // Need to read_string the rest of the packet here regardless
			EthernetUDP_flush(&dhcp->_dhcpUdpSocket);
            return 0;
        }

        memcpy(dhcp->_dhcpLocalIp, fixedMsg.yiaddr, 4);

        // Skip to the option part
        // Doing this a byte at a time so we don't have to put a big buffer
        // on the stack (as we don't have lots of memory lying around)
        for (int i =0; i < (240 - (int)sizeof(RIP_MSG_FIXED)); i++)
        {
			EthernetUDP_read(&dhcp->_dhcpUdpSocket);
        }

        while (EthernetUDP_available(&dhcp->_dhcpUdpSocket) > 0)
        {
            switch (EthernetUDP_read(&dhcp->_dhcpUdpSocket))
            {
                case endOption :
                    break;
                    
                case padOption :
                    break;
                
                case dhcpMessageType :
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                    type = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                    break;
                
                case subnetMask :
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
					EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, dhcp->_dhcpSubnetMask, 4);
                    break;
                
                case routersOnSubnet :
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
					EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, dhcp->_dhcpGatewayIp, 4);
                    for (int i = 0; i < opt_len-4; i++)
                    {
						EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                    }
                    break;
                
                case dns :
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
					EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, dhcp->_dhcpDnsServerIp, 4);
                    for (int i = 0; i < opt_len-4; i++)
                    {
						EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                    }
                    break;
                
                case dhcpServerIdentifier :
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                    if( *((uint32_t*)dhcp->_dhcpDhcpServerIp) == 0 ||
						*((uint32_t*)dhcp->_dhcpDhcpServerIp) == dhcp->_dhcpUdpSocket._remoteIP.dword )
                    {
						EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, dhcp->_dhcpDhcpServerIp, sizeof(dhcp->_dhcpDhcpServerIp));
                    }
                    else
                    {
                        // Skip over the rest of this option
                        while (opt_len--)
                        {
							EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                        }
                    }
                    break;

                case dhcpT1value : 
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
					EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, (uint8_t*)&dhcp->_dhcpT1, sizeof(dhcp->_dhcpT1));
					dhcp->_dhcpT1 = ntohl(dhcp->_dhcpT1);
                    break;

                case dhcpT2value : 
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
					EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, (uint8_t*)&dhcp->_dhcpT2, sizeof(dhcp->_dhcpT2));
					dhcp->_dhcpT2 = ntohl(dhcp->_dhcpT2);
                    break;

                case dhcpIPaddrLeaseTime :
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
					EthernetUDP_read_buf(&dhcp->_dhcpUdpSocket, (uint8_t*)&dhcp->_dhcpLeaseTime, sizeof(dhcp->_dhcpLeaseTime));
					dhcp->_dhcpLeaseTime = ntohl(dhcp->_dhcpLeaseTime);
					dhcp->_renewInSec = dhcp->_dhcpLeaseTime;
                    break;

                default :
                    opt_len = EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                    // Skip over the rest of this option
                    while (opt_len--)
                    {
                        EthernetUDP_read(&dhcp->_dhcpUdpSocket);
                    }
                    break;
            }
        }
    }

    // Need to skip to end of the packet regardless here
	EthernetUDP_flush(&dhcp->_dhcpUdpSocket);

    return type;
}


/*
    returns:
    0/DHCP_CHECK_NONE: nothing happened
    1/DHCP_CHECK_RENEW_FAIL: renew failed
    2/DHCP_CHECK_RENEW_OK: renew success
    3/DHCP_CHECK_REBIND_FAIL: rebind fail
    4/DHCP_CHECK_REBIND_OK: rebind success
*/
int DHCP_checkLease(DHCP* dhcp){
    //this uses a signed / unsigned trick to deal with millis overflow
    unsigned long now = millis();
    signed long snow = (long)now;
    int rc=DHCP_CHECK_NONE;
    if (dhcp->_lastCheck != 0){
        signed long factor;
        //calc how many ms past the timeout we are
        factor = snow - (long)dhcp->_secTimeout;
        //if on or passed the timeout, reduce the counters
        if ( factor >= 0 ){
            //next timeout should be now plus 1000 ms minus parts of second in factor
			dhcp->_secTimeout = snow + 1000 - factor % 1000;
            //how many seconds late are we, minimum 1
            factor = factor / 1000 +1;
            
            //reduce the counters by that mouch
            //if we can assume that the cycle time (factor) is fairly constant
            //and if the remainder is less than cycle time * 2 
            //do it early instead of late
            if(dhcp->_renewInSec < factor*2 )
				dhcp->_renewInSec = 0;
            else
				dhcp->_renewInSec -= factor;
            
            if(dhcp->_rebindInSec < factor*2 )
				dhcp->_rebindInSec = 0;
            else
				dhcp->_rebindInSec -= factor;
        }

        //if we have a lease but should renew, do it
        if (dhcp->_dhcp_state == STATE_DHCP_LEASED && dhcp->_renewInSec <=0){
			dhcp->_dhcp_state = STATE_DHCP_REREQUEST;
            rc = 1 +DHCP_request_DHCP_lease(dhcp);
        }

        //if we have a lease or is renewing but should bind, do it
        if( (dhcp->_dhcp_state == STATE_DHCP_LEASED || dhcp->_dhcp_state == STATE_DHCP_START) && dhcp->_rebindInSec <=0){
            //this should basically restart completely
			dhcp->_dhcp_state = STATE_DHCP_START;
           DHCP_reset_DHCP_lease(dhcp);
            rc = 3 + DHCP_request_DHCP_lease(dhcp);
        }
    }
    else{
		dhcp->_secTimeout = snow + 1000;
    }

	dhcp->_lastCheck = now;
    return rc;
}

IPAddress DHCP_getLocalIp(DHCP* dhcp)
{
	IPAddress ip;
	IPAddress_Init_str(&ip, dhcp->_dhcpLocalIp);
    return ip;
}

IPAddress DHCP_getSubnetMask(DHCP* dhcp)
{
	IPAddress ip;
	IPAddress_Init_str(&ip, dhcp->_dhcpSubnetMask);
	return ip;
}

IPAddress DHCP_getGatewayIp(DHCP* dhcp)
{
	IPAddress ip;
	IPAddress_Init_str(&ip, dhcp->_dhcpGatewayIp);
	return ip;
}

IPAddress DHCP_getDhcpServerIp(DHCP* dhcp)
{
	IPAddress ip;
	IPAddress_Init_str(&ip, dhcp->_dhcpDhcpServerIp);
	return ip;
}

IPAddress DHCP_getDnsServerIp(DHCP* dhcp)
{
	IPAddress ip;
	IPAddress_Init_str(&ip, dhcp->_dhcpDnsServerIp);
	return ip;
}

void DHCP_printByte(char * buf, uint8_t n ) {
  char *str = &buf[1];
  buf[0]='0';
  do {
    unsigned long m = n;
    n /= 16;
    char c = m - 16 * n;
    *str-- = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);
}
