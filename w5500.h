/*
* Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef	W5500_H_INCLUDED
#define	W5500_H_INCLUDED

#include "SPIfg.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef uint8_t SOCKET;

#define MAX_SOCK_NUM 8


#define SnMR_CLOSE   0x00
#define  SnMR_TCP    0x01
#define SnMR_UDP     0x02
#define SnMR_IPRAW   0x03
#define SnMR_MACRAW  0x04
#define SnMR_PPPOE   0x05
#define SnMR_ND      0x20
#define SnMR_MULTI   0x80

typedef enum SockCMD {
  Sock_OPEN      = 0x01,
  Sock_LISTEN    = 0x02,
  Sock_CONNECT   = 0x04,
  Sock_DISCON    = 0x08,
  Sock_CLOSE     = 0x10,
  Sock_SEND      = 0x20,
  Sock_SEND_MAC  = 0x21,
  Sock_SEND_KEEP = 0x22,
  Sock_RECV      = 0x40
} SockCMD;


#define SnIR_SEND_OK  0x10
#define SnIR_TIMEOUT  0x08
#define SnIR_RECV     0x04
#define SnIR_DISCON   0x02
#define SnIR_CON      0x01

#define SnSR_CLOSED       0x00
#define SnSR_INIT         0x13
#define SnSR_LISTEN       0x14
#define SnSR_SYNSENT      0x15
#define SnSR_SYNRECV      0x16
#define SnSR_ESTABLISHED  0x17
#define SnSR_FIN_WAIT     0x18
#define SnSR_CLOSING      0x1A
#define SnSR_TIME_WAIT    0x1B
#define SnSR_CLOSE_WAIT   0x1C
#define SnSR_LAST_ACK     0x1D
#define SnSR_UDP          0x22
#define SnSR_IPRAW        0x32
#define SnSR_MACRAW       0x42
#define SnSR_PPPOE        0x5F

#define IPPROTO_IP    0
#define IPPROTO_ICMP  1
#define IPPROTO_IGMP  2;
#define IPPROTO_GGP   3
#define IPPROTO_TCP   6
#define IPPROTO_PUP   12
#define IPPROTO_UDP   17
#define IPPROTO_IDP   22
#define IPPROTO_ND    77
#define IPPROTO_RAW   255


  void W5500_init();

  /**
   * @brief	This function is being used for copy the data form Receive buffer of the chip to application buffer.
   * 
   * It calculate the actual physical address where one has to read_string
   * the data from Receive buffer. Here also take care of the condition while it exceed
   * the Rx memory uper-bound of socket.
   */
  void W5500_read_data(SOCKET s, volatile uint16_t  src, volatile uint8_t * dst, uint16_t len);
  
  /**
   * @brief	 This function is being called by send() and sendto() function also. 
   * 
   * This function read_string the Tx write_byte pointer register and after copy the data in buffer update the Tx write_byte pointer
   * register. User should read_string upper byte first and lower byte later to get proper value.
   */
  void W5500_send_data_processing(SOCKET s, const uint8_t *data, uint16_t len);
  /**
   * @brief A copy of send_data_processing that uses the provided ptr for the
   *        write_byte offset.  Only needed for the "streaming" UDP API, where
   *        a single UDP packet is built up over a number of calls to
   *        send_data_processing_ptr, because TX_WR doesn't seem to get updated
   *        correctly in those scenarios
   * @param ptr value to use in place of TX_WR.  If 0, then the value is read_string
   *        in from TX_WR
   * @return New value for ptr, to be used in the next call
   */
  // FIXME Update documentation
  void W5500_send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len);

  /**
   * @brief	This function is being called by recv() also.
   * 
   * This function read_string the Rx read_string pointer register
   * and after copy the data from receive buffer update the Rx write_byte pointer register.
   * User should read_string upper byte first and lower byte later to get proper value.
   */
  void W5500_recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek);

   void W5500_setGatewayIp(uint8_t *_addr);
   void W5500_getGatewayIp(uint8_t *_addr);

   void W5500_setSubnetMask(uint8_t *_addr);
   void W5500_getSubnetMask(uint8_t *_addr);

   void W5500_setMACAddress(uint8_t * addr);
   void W5500_getMACAddress(uint8_t * addr);

   void W5500_setIPAddress(uint8_t * addr);
   void W5500_getIPAddress(uint8_t * addr);

   void W5500_setRetransmissionTime(uint16_t timeout);
   void W5500_setRetransmissionCount(uint8_t _retry);

   void W5500_setPHYCFGR(uint8_t _val);
   uint8_t W5500_getPHYCFGR();

  void W5500_execCmdSn(SOCKET s, SockCMD _cmd);
  
  uint16_t W5500_getTXFreeSize(SOCKET s);
  uint16_t W5500_getRXReceivedSize(SOCKET s);
  

  // W5500 Registers
  // ---------------
   uint8_t  W5500_write(uint16_t _addr, uint8_t _cb, uint8_t _data);
   uint16_t W5500_write_buf(uint16_t _addr, uint8_t _cb, const uint8_t *buf, uint16_t len);
   uint8_t  W5500_read(uint16_t _addr, uint8_t _cb );
   uint16_t W5500_read_buf(uint16_t _addr, uint8_t _cb, uint8_t *buf, uint16_t len);

#define __GP_REGISTER8(name, address)             \
 void W5500_write##name(uint8_t _data) { \
    W5500_write(address, 0x04, _data);                  \
  }                                               \
 uint8_t W5500_read##name() {            \
    return W5500_read(address, 0x00);                   \
  }

#define __GP_REGISTER8_PROTO(name)             \
 void W5500_write##name(uint8_t _data);                  \
 uint8_t W5500_read##name();  


#define __GP_REGISTER16(name, address)            \
   void W5500_write##name(uint16_t _data) {       \
    W5500_write(address,  0x04, _data >> 8);            \
    W5500_write(address+1, 0x04, _data & 0xFF);         \
  }                                               \
   uint16_t read##name() {                  \
    uint16_t res = W5500_read(address, 0x00);           \
    res = (res << 8) + W5500_read(address + 1, 0x00);   \
    return res;                                   \
  }
#define __GP_REGISTER16_PROTO(name)            \
   void W5500_write##name(uint16_t _data);       \
   uint16_t read##name();                     


#define __GP_REGISTER_N(name, address, size)      \
   uint16_t W5500_write##name(uint8_t *_buff) {   \
    return W5500_write_buf(address, 0x04, _buff, size);     \
  }                                               \
   uint16_t W5500_read##name(uint8_t *_buff) {    \
    return W5500_read_buf(address, 0x00, _buff, size);      \
  }

#define __GP_REGISTER_N_PROTO(name)      \
   uint16_t W5500_read##name(uint8_t *_buff);  \
   uint16_t W5500_write##name(uint8_t *_buff);


   __GP_REGISTER8_PROTO(MR);    // Mode
   __GP_REGISTER_N_PROTO(GAR); // Gateway IP address
   __GP_REGISTER_N_PROTO(SUBR); // Subnet mask address
   __GP_REGISTER_N_PROTO(SHAR); // Source MAC address
   __GP_REGISTER_N_PROTO(SIPR); // Source IP address
   __GP_REGISTER8_PROTO(IR);    // Interrupt
   __GP_REGISTER8_PROTO(IMR);    // Interrupt Mask
   __GP_REGISTER16_PROTO(RTR);    // Timeout address
   __GP_REGISTER8_PROTO(RCR);    // Retry count
   __GP_REGISTER_N_PROTO(UIPR); // Unreachable IP address in UDP mode
   __GP_REGISTER16_PROTO(UPORT);    // Unreachable Port address in UDP mode
   __GP_REGISTER8_PROTO(PHYCFGR);    // PHY Configuration register, default value: 0b 1011 1xxx

  

  // W5500 Socket registers
  // ----------------------
 uint8_t W5500_readSn(SOCKET _s, uint16_t _addr);
 uint8_t W5500_writeSn(SOCKET _s, uint16_t _addr, uint8_t _data);
 uint16_t W5500_readSn_buf(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);
 uint16_t W5500_writeSn_buf(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);

 // const uint16_t CH_BASE = 0x0000;
 // const uint16_t CH_SIZE = 0x0000;

#define __SOCKET_REGISTER8(name, address)                    \
 void W5500_write##name(SOCKET _s, uint8_t _data) { \
    W5500_writeSn(_s, address, _data);                             \
  }                                                          \
 uint8_t W5500_read##name(SOCKET _s) {              \
    return W5500_readSn(_s, address);                              \
  }

#define __SOCKET_REGISTER8_PROTO(name)					\
 void W5500_write##name(SOCKET _s, uint8_t _data); uint8_t W5500_read##name(SOCKET _s);

#define __SOCKET_REGISTER16(name, address)                   \
   void W5500_write##name(SOCKET _s, uint16_t _data) {       \
    W5500_writeSn(_s, address,   _data >> 8);                      \
    W5500_writeSn(_s, address+1, _data & 0xFF);                    \
  }                                                          \
   uint16_t W5500_read##name(SOCKET _s) {                    \
    uint16_t res = W5500_readSn(_s, address);                      \
    res = (res << 8) + W5500_readSn(_s, address + 1);              \
    return res;                                              \
  }

#define __SOCKET_REGISTER16_PROTO(name)				\
 void W5500_write##name(SOCKET _s, uint16_t _data);	uint16_t W5500_read##name(SOCKET _s);


#define __SOCKET_REGISTER_N(name, address, size)			\
   uint16_t W5500_write##name(SOCKET _s, uint8_t *_buff) {	\
    return W5500_writeSn_buf(_s, address, _buff, size);			\
  }															\
   uint16_t W5500_read##name(SOCKET _s, uint8_t *_buff) {	\
    return W5500_readSn_buf(_s, address, _buff, size);			\
  }

#define __SOCKET_REGISTER_N_PROTO(name)						\
 uint16_t W5500_write##name(SOCKET _s, uint8_t *_buff);		 uint16_t W5500_read##name(SOCKET _s, uint8_t *_buff);


     __SOCKET_REGISTER8_PROTO(SnMR);        // Mode
     __SOCKET_REGISTER8_PROTO(SnCR);        // Command
	 __SOCKET_REGISTER8_PROTO(SnIR)        // Interrupt
	 __SOCKET_REGISTER8_PROTO(SnSR)        // Status
	 __SOCKET_REGISTER16_PROTO(SnPORT)        // Source Port
	 __SOCKET_REGISTER_N_PROTO(SnDHAR)     // Destination Hardw Addr
	 __SOCKET_REGISTER_N_PROTO(SnDIPR)     // Destination IP Addr
	 __SOCKET_REGISTER16_PROTO(SnDPORT)        // Destination Port
	 __SOCKET_REGISTER16_PROTO(SnMSSR)        // Max Segment Size
	 __SOCKET_REGISTER8_PROTO(SnPROTO)        // Protocol in IP RAW Mode
	 __SOCKET_REGISTER8_PROTO(SnTOS)        // IP TOS
	 __SOCKET_REGISTER8_PROTO(SnTTL)        // IP TTL
	 __SOCKET_REGISTER16_PROTO(SnTX_FSR)        // TX Free Size
	 __SOCKET_REGISTER16_PROTO(SnTX_RD)        // TX Read Pointer
	 __SOCKET_REGISTER16_PROTO(SnTX_WR)        // TX Write Pointer
	 __SOCKET_REGISTER16_PROTO(SnRX_RSR)        // RX Free Size
	 __SOCKET_REGISTER16_PROTO(SnRX_RD)        // RX Read Pointer
	 __SOCKET_REGISTER16_PROTO(SnRX_WR)        // RX Write Pointer (supported?)


#define  W5500_RST 7 // Reset BIT
#define  W5500_SOCKETS  8


#define W5500_SSIZE  (uint16_t)2048 // Max Tx buffer size
#define W5500_RSIZE  (uint16_t)2048 // Max Rx buffer size

#ifdef __cplusplus
}
#endif


#endif
