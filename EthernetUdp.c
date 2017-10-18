/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.c/socket.h
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/ 
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#include "W5500.h"
#include "socket.h"
#include "Ethernet.h"
#include "Dns.h"

void EthernetUDP_begin_init(EthernetUDP* eth) {
	eth->_sock = MAX_SOCK_NUM;
}

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t EthernetUDP_begin(EthernetUDP* eth, uint16_t port) {
  if (eth->_sock != MAX_SOCK_NUM)
    return 0;

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5500_readSnSR(i);
    if (s == SnSR_CLOSED || s == SnSR_FIN_WAIT) {
      eth->_sock = i;
      break;
    }
  }

  if (eth->_sock == MAX_SOCK_NUM)
    return 0;

  eth->_port = port;
  eth->_remaining = 0;
  socket(eth->_sock, SnMR_UDP, eth->_port, 0);

  return 1;
}

/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int EthernetUDP_available(EthernetUDP* eth) {
  return eth->_remaining;
}

/* Release any resources being used by this EthernetUDP instance */
void EthernetUDP_stop(EthernetUDP* eth)
{
  if (eth->_sock == MAX_SOCK_NUM)
    return;

  close(eth->_sock);

  Ethernet_server_port[eth->_sock] = 0;
  eth->_sock = MAX_SOCK_NUM;
}

int EthernetUDP_beginPacket_host(EthernetUDP* eth, const char *host, uint16_t port)
{
  // Look up the host first
  int ret = 0;
  DNSClient dns;
  IPAddress remote_addr;

  DNSClient_begin(&dns,Ethernet_dnsServerIP());
  ret = DNSClient_getHostByName(&dns, host, &remote_addr);
  if (ret == 1) {
    return EthernetUDP_beginPacket_ip(eth,remote_addr, port);
  } else {
    return ret;
  }
}

int EthernetUDP_beginPacket_ip(EthernetUDP* eth, IPAddress ip, uint16_t port)
{
  eth->_offset = 0;
  return startUDP(eth->_sock, EthernetUDP_rawIPAddress(eth, &ip), port);
}

int EthernetUDP_endPacket(EthernetUDP* eth)
{
  return sendUDP(eth->_sock);
}

size_t EthernetUDP_write(void* eth, const uint8_t *buffer, size_t size)
{
  uint16_t bytes_written = bufferData(((EthernetUDP*)eth)->_sock, ((EthernetUDP*)eth)->_offset, buffer, size);
  ((EthernetUDP*)eth)->_offset += bytes_written;
  return bytes_written;
}

int EthernetUDP_parsePacket(EthernetUDP* eth)
{
  // discard any remaining bytes in the last packet
  EthernetUDP_flush(eth);

  if (W5500_getRXReceivedSize(eth->_sock) > 0)
  {
    //HACK - hand-parse the UDP packet using TCP recv method
    uint8_t tmpBuf[8];
    int ret =0; 
    //read_string 8 header bytes and get IP and port from it
    ret = recv(eth->_sock,tmpBuf,8);
    if (ret > 0)
    {
	  IPAddress_Init_str(&eth->_remoteIP, tmpBuf);
      eth->_remotePort = tmpBuf[4];
      eth->_remotePort = (eth->_remotePort << 8) + tmpBuf[5];
      eth->_remaining = tmpBuf[6];
      eth->_remaining = (eth->_remaining << 8) + tmpBuf[7];

      // When we get here, any remaining bytes are the data
      ret = eth->_remaining;
    }
    return ret;
  }
  // There aren't any packets available
  return 0;
}

int EthernetUDP_read(EthernetUDP* eth)
{
  uint8_t byte;

  if ((eth->_remaining > 0) && (recv(eth->_sock, &byte, 1) > 0))
  {
    // We read_string things without any problems
    eth->_remaining--;
    return byte;
  }

  // If we get here, there's no data available
  return -1;
}

int EthernetUDP_read_buf(EthernetUDP* eth, uint8_t* buffer, size_t len)
{

  if (eth->_remaining > 0)
  {

    int got;

    if (eth->_remaining <= len)
    {
      // data should fit in the buffer
      got = recv(eth->_sock, buffer, eth->_remaining);
    }
    else
    {
      // too much data for the buffer, 
      // grab as much as will fit
      got = recv(eth->_sock, buffer, len);
    }

    if (got > 0)
    {
      eth->_remaining -= got;
      return got;
    }

  }

  // If we get here, there's no data available or recv failed
  return -1;

}

int EthernetUDP_peek(EthernetUDP* eth)
{
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must.
  // If the user hasn't called parsePacket yet then return nothing otherwise they
  // may get the UDP header
  if (!eth->_remaining)
    return -1;
  peek(eth->_sock, &b);
  return b;
}

void EthernetUDP_flush(EthernetUDP* eth)
{
  // could this fail (loop endlessly) if eth->_remaining > 0 and recv in read_string fails?
  // should only occur if recv fails after telling us the data is there, lets
  // hope the w5100 always behaves :)

  while (eth->_remaining)
  {
	  EthernetUDP_read(eth);
  }
}

uint8_t EthernetUDP_beginMulticast(EthernetUDP* eth, IPAddress* ip, uint16_t port) {

	for (int i = 0; i < MAX_SOCK_NUM; i++) {
		uint8_t s = W5500_readSnSR(i);
		if (s == SnSR_CLOSED || s == SnSR_FIN_WAIT) {
			eth->_sock = i;
			break;
		}
	}

	if (eth->_sock == MAX_SOCK_NUM)
		return 0;

	uint8_t mac[] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };

	mac[3] = ip->bytes[1] & 0x7F;
	mac[4] = ip->bytes[2];
	mac[5] = ip->bytes[3];

	//  byte mac[] = { 0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb };

	W5500_writeSnDIPR(eth->_sock, EthernetUDP_rawIPAddress(eth,ip));
	W5500_writeSnDPORT(eth->_sock, port);
	W5500_writeSnDHAR(eth->_sock, mac);

	eth->_remaining = 0;

	socket(eth->_sock, SnMR_UDP, port, SnMR_MULTI);

	return 1;
}

// Read up to len characters from the current packet and place them into buffer
// Returns the number of characters read_string, or 0 if none are available
int EthernetUDP_read_string(EthernetUDP* eth, char* buffer, size_t len) { return EthernetUDP_read_buf(eth, (uint8_t*)buffer, len); };

											// Return the IP address of the host who sent the current incoming packet
IPAddress EthernetUDP_remoteIP(EthernetUDP* eth) { return eth->_remoteIP; };
// Return the port of the host who sent the current incoming packet
uint16_t EthernetUDP_remotePort(EthernetUDP* eth) { return eth->_remotePort; };

uint8_t* EthernetUDP_rawIPAddress(EthernetUDP* eth, IPAddress* addr) { return addr->bytes; };


GENERATE_PRINT_INSTANCE(EthernetUDP)
