/*
 * Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <stdio.h>

#include "W5500.h"

#ifndef ARDUINO
#include "ArduinoCompatibility.h"
#endif

#define SPI_CS 10

void W5500_init(void)
{
    initSS();
    SPI_begin();
    for (int i=0; i<MAX_SOCK_NUM; i++) {
        uint8_t cntl_byte = (0x0C + (i<<5));
		W5500_write( 0x1E, cntl_byte, 2); //0x1E - Sn_RXBUF_SIZE
		W5500_write( 0x1F, cntl_byte, 2); //0x1F - Sn_TXBUF_SIZE
    }
}

uint16_t W5500_getTXFreeSize(SOCKET s)
{
    uint16_t val=0, val1=0;
    do {
        val1 = W5500_readSnTX_FSR(s);
        if (val1 != 0)
            val = W5500_readSnTX_FSR(s);
    } 
    while (val != val1);
    return val;
}

uint16_t W5500_getRXReceivedSize(SOCKET s)
{
  volatile   uint16_t val=0,val1=0;
    do {
        val1 = W5500_readSnRX_RSR(s);
        if (val1 != 0)
            val = W5500_readSnRX_RSR(s);
    } 
    while (val != val1);
    return val;
}

void W5500_send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
  // This is same as having no offset in a call to send_data_processing_offset
	W5500_send_data_processing_offset(s, 0, data, len);

}

void W5500_send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{

    uint16_t ptr = W5500_readSnTX_WR(s);
    uint8_t cntl_byte = (0x14+(s<<5));
    ptr += data_offset;
	W5500_write_buf(ptr, cntl_byte, data, len);
    ptr += len;
	W5500_writeSnTX_WR(s, ptr);
    
}

void W5500_recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
    uint16_t ptr;
    ptr = W5500_readSnRX_RD(s);

	W5500_read_data(s, ptr, data, len);
    if (!peek)
    {
        ptr += len;
		W5500_writeSnRX_RD(s, ptr);
    }
}

void W5500_read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len)
{
    uint8_t cntl_byte = (0x18+(s<<5));
	W5500_read_buf((uint16_t)src , cntl_byte, (uint8_t *)dst, len);
}

uint8_t W5500_write(uint16_t _addr, uint8_t _cb, uint8_t _data)
{
    setSS();  
    SPI_transf(_addr >> 8);
    SPI_transf(_addr & 0xFF);
    SPI_transf(_cb);
    SPI_transf(_data);
    resetSS();
    return 1;
}

uint16_t W5500_write_buf(uint16_t _addr, uint8_t _cb, const uint8_t *_buf, uint16_t _len)
{
    setSS();
    SPI_transf(_addr >> 8);
    SPI_transf(_addr & 0xFF);
    SPI_transf(_cb);
    for (uint16_t i=0; i<_len; i++){
        SPI_transf(_buf[i]);
    }
    resetSS();
    return _len;
}

uint8_t W5500_read(uint16_t _addr, uint8_t _cb)
{
    setSS();
    SPI_transf(_addr >> 8);
    SPI_transf(_addr & 0xFF);
    SPI_transf(_cb);
    uint8_t _data = SPI_transf(0);
    resetSS();
    return _data;
}

uint16_t W5500_read_buf(uint16_t _addr, uint8_t _cb, uint8_t *_buf, uint16_t _len)
{ 
    setSS(); 
    SPI_transf(_addr >> 8);
    SPI_transf(_addr & 0xFF);
    SPI_transf(_cb);
    for (uint16_t i=0; i<_len; i++){
        _buf[i] = SPI_transf(0);
    }
    resetSS();
    return _len;
}

void W5500_execCmdSn(SOCKET s, SockCMD _cmd) {
    // Send command to socket
	W5500_writeSnCR(s, _cmd);
    // Wait for command to complete
    while (W5500_readSnCR(s))
    ;
}

uint8_t W5500_readSn(SOCKET _s, uint16_t _addr) {
	uint8_t cntl_byte = (_s << 5) + 0x08;
	return W5500_read(_addr, cntl_byte);
}

uint8_t W5500_writeSn(SOCKET _s, uint16_t _addr, uint8_t _data) {
	uint8_t cntl_byte = (_s << 5) + 0x0C;
	return W5500_write(_addr, cntl_byte, _data);
}

uint16_t W5500_readSn_buf(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
	uint8_t cntl_byte = (_s << 5) + 0x08;
	return W5500_read_buf(_addr, cntl_byte, _buf, _len);
}

uint16_t W5500_writeSn_buf(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
	uint8_t cntl_byte = (_s << 5) + 0x0C;
	return W5500_write_buf(_addr, cntl_byte, _buf, _len);
}

void W5500_getGatewayIp(uint8_t *_addr) {
	W5500_readGAR(_addr);
}

void W5500_setGatewayIp(uint8_t *_addr) {
	W5500_writeGAR(_addr);
}

void W5500_getSubnetMask(uint8_t *_addr) {
	W5500_readSUBR(_addr);
}

void W5500_setSubnetMask(uint8_t *_addr) {
	W5500_writeSUBR(_addr);
}

void W5500_getMACAddress(uint8_t *_addr) {
	W5500_readSHAR(_addr);
}

void W5500_setMACAddress(uint8_t *_addr) {
	W5500_writeSHAR(_addr);
}

void W5500_getIPAddress(uint8_t *_addr) {
	W5500_readSIPR(_addr);
}

void W5500_setIPAddress(uint8_t *_addr) {
	W5500_writeSIPR(_addr);
}

void W5500_setRetransmissionTime(uint16_t _timeout) {
	W5500_writeRTR(_timeout);
}

void W5500_setRetransmissionCount(uint8_t _retry) {
	W5500_writeRCR(_retry);
}

void W5500_setPHYCFGR(uint8_t _val) {
	W5500_writePHYCFGR(_val);
}

uint8_t W5500_getPHYCFGR() {
	//  readPHYCFGR();
	return W5500_read(0x002E, 0x00);
}

__GP_REGISTER8(MR, 0x0000);    // Mode
__GP_REGISTER_N(GAR, 0x0001, 4); // Gateway IP address
__GP_REGISTER_N(SUBR, 0x0005, 4); // Subnet mask address
__GP_REGISTER_N(SHAR, 0x0009, 6); // Source MAC address
__GP_REGISTER_N(SIPR, 0x000F, 4); // Source IP address
__GP_REGISTER8(IR, 0x0015);    // Interrupt
__GP_REGISTER8(IMR, 0x0016);    // Interrupt Mask
__GP_REGISTER16(RTR, 0x0019);    // Timeout address
__GP_REGISTER8(RCR, 0x001B);    // Retry count
__GP_REGISTER_N(UIPR, 0x0028, 4); // Unreachable IP address in UDP mode
__GP_REGISTER16(UPORT, 0x002C);    // Unreachable Port address in UDP mode
__GP_REGISTER8(PHYCFGR, 0x002E);    // PHY Configuration register, default value: 0b 1011 1xxx


__SOCKET_REGISTER8(SnMR, 0x0000)        // Mode
__SOCKET_REGISTER8(SnCR, 0x0001)        // Command
__SOCKET_REGISTER8(SnIR, 0x0002)        // Interrupt
__SOCKET_REGISTER8(SnSR, 0x0003)        // Status
__SOCKET_REGISTER16(SnPORT, 0x0004)        // Source Port
__SOCKET_REGISTER_N(SnDHAR, 0x0006, 6)     // Destination Hardw Addr
__SOCKET_REGISTER_N(SnDIPR, 0x000C, 4)     // Destination IP Addr
__SOCKET_REGISTER16(SnDPORT, 0x0010)        // Destination Port
__SOCKET_REGISTER16(SnMSSR, 0x0012)        // Max Segment Size
__SOCKET_REGISTER8(SnPROTO, 0x0014)        // Protocol in IP RAW Mode
__SOCKET_REGISTER8(SnTOS, 0x0015)        // IP TOS
__SOCKET_REGISTER8(SnTTL, 0x0016)        // IP TTL
__SOCKET_REGISTER16(SnTX_FSR, 0x0020)        // TX Free Size
__SOCKET_REGISTER16(SnTX_RD, 0x0022)        // TX Read Pointer
__SOCKET_REGISTER16(SnTX_WR, 0x0024)        // TX Write Pointer
__SOCKET_REGISTER16(SnRX_RSR, 0x0026)        // RX Free Size
__SOCKET_REGISTER16(SnRX_RD, 0x0028)        // RX Read Pointer
__SOCKET_REGISTER16(SnRX_WR, 0x002A)        // RX Write Pointer (supported?)
