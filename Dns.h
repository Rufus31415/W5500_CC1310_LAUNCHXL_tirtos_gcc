// Arduino DNS client for WizNet5100-based Ethernet shield
// (c) Copyright 2009-2010 MCQN Ltd.
// Released under Apache License, version 2.0

#ifndef DNSClient_h
#define DNSClient_h

#include "EthernetUdp.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct DNSClient {
	IPAddress iDNSServer;
	uint16_t iRequestId;
	EthernetUDP iUdp;
} DNSClient;



    void DNSClient_begin(DNSClient* dns, IPAddress aDNSServer);

    /** Convert a numeric IP address string into a four-byte IP address.
        @param aIPAddrString IP address to convert
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int DNSClient_inet_aton(const char *aIPAddrString, IPAddress* aResult);

    /** Resolve the given hostname to an IP address.
        @param aHostname Name to be resolved
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int DNSClient_getHostByName(DNSClient* dns, const char* aHostname, IPAddress* aResult);

    uint16_t DNSClient_BuildRequest(DNSClient* dns, const char* aName);
    uint16_t DNSClient_ProcessResponse(DNSClient* dns, uint16_t aTimeout, IPAddress* aAddress);

#ifdef __cplusplus
}
#endif

#endif
