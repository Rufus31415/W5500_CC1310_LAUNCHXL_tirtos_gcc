#include "W5500.h"
#include "socket.h"
//extern "C" {
#include "string.h"
//}

#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"


void EthernetServer_begin(EthernetServer* eth, uint16_t port)
{
  eth->_port = port;

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    EthernetClient client;
	EthernetClient_begin(&client, sock);
    if (EthernetClient_status(&client) == SnSR_CLOSED) {
      socket(sock, SnMR_TCP, eth->_port, 0);
      listen(sock);
      Ethernet_server_port[sock] = eth->_port;
      break;
    }
  }  
}

void EthernetServer_accept(EthernetServer* eth)
{
  int listening = 0;

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
	  EthernetClient client;
	  EthernetClient_begin(&client, sock);

    if (Ethernet_server_port[sock] == eth->_port) {
      if (EthernetClient_status(&client) == SnSR_LISTEN) {
        listening = 1;
      } 
      else if (EthernetClient_status(&client) == SnSR_CLOSE_WAIT && !EthernetClient_available(&client)) {
		  EthernetClient_stop(&client);
      }
    } 
  }

  if (!listening) {
	  EthernetServer_begin(eth, eth->_port);
  }
}

EthernetClient EthernetServer_available(EthernetServer* eth)
{
	EthernetServer_accept(eth);

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
	  EthernetClient client;
	  EthernetClient_begin(&client, sock);
	  
	  if (Ethernet_server_port[sock] == eth->_port &&
        (EthernetClient_status(&client) == SnSR_ESTABLISHED ||
			EthernetClient_status(&client) == SnSR_CLOSE_WAIT)) {
      if (EthernetClient_available(&client)) {
        // XXX: don't always pick the lowest numbered socket.
        return client;
      }
    }
  }

  EthernetClient cl;
  EthernetClient_begin(&cl, MAX_SOCK_NUM);
  return cl;
}


size_t EthernetServer_write(void* eth, const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  
  EthernetServer_accept((EthernetServer*)eth);

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
	  EthernetClient client;
	  EthernetClient_begin(&client, sock);

    if (Ethernet_server_port[sock] == ((EthernetServer*)eth)->_port &&
		EthernetClient_status(&client) == SnSR_ESTABLISHED) {
      n += EthernetClient_write(&client, buffer, size);
    }
  }
  
  return n;
}

GENERATE_PRINT_INSTANCE(EthernetServer)