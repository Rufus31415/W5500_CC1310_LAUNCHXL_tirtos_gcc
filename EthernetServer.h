#ifndef ethernetserver_h
#define ethernetserver_h

#ifdef __cplusplus
extern "C" {
#endif


typedef struct EthernetServer_ {
	uint16_t _port;
} EthernetServer;

  void EthernetServer_accept(EthernetServer*);
  void EthernetServer_begin(EthernetServer*, uint16_t);
  EthernetClient EthernetServer_available(EthernetServer*);


GENERATE_PRINT_HEADER(EthernetServer);

#ifdef __cplusplus
}
#endif

#endif
