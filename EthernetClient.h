#ifndef ethernetclient_h
#define ethernetclient_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "IPAddress.h"
#include "printfg.h"

#ifdef __cplusplus
extern "C" {
#endif


// This enumeration provides the lookahead options for parseInt_protected(), parseFloat()
// The rules set out here are used until either the first valid character is found
// or a time out occurs due to lack of input.
typedef enum LookaheadModefg {
	SKIP_ALLfg,       // All invalid characters are ignored.
	SKIP_NONEfg,      // Nothing is skipped, and the stream is not touched unless the first waiting character is valid.
	SKIP_WHITESPACEfg // Only tabs, spaces, line feeds & carriage returns are skipped.
} LookaheadModefg;

typedef	struct MultiTarget {
		const char *str;  // string you're searching for
		size_t len;       // length of string you're searching for
		size_t index;     // index used by the search routine.
}MultiTarget;

#define NO_IGNORE_CHAR  '\x01' // a char not found in a valid ASCII numeric field

extern uint16_t EthernetClient_srcport;

  typedef struct EthernetClient_ {
	  uint8_t _sock;
	  unsigned long _timeout;      // number of milliseconds to wait for the next char before aborting timed read_string
	  unsigned long _startMillis;  // used for timeout measurement
  } EthernetClient;


  void EthernetClient_begin_default(EthernetClient* eth);

  void EthernetClient_begin(EthernetClient* eth, uint8_t sock);

  uint8_t EthernetClient_status(EthernetClient* eth);
  int EthernetClient_connect_ip(EthernetClient* eth, IPAddress ip, uint16_t port);
  int EthernetClient_connect_host(EthernetClient* eth, const char *host, uint16_t port);
  size_t EthernetClient_write(void* eth, const uint8_t *buf, size_t size);
  int EthernetClient_available(EthernetClient* eth);
  int EthernetClient_read(EthernetClient* eth);
  int EthernetClient_read_buf(EthernetClient* eth, uint8_t *buf, size_t size);
  int EthernetClient_peek(EthernetClient* eth);
  void EthernetClient_flush(EthernetClient* eth);
  void EthernetClient_stop(EthernetClient* eth);
  uint8_t EthernetClient_connected(EthernetClient* eth);

  int EthernetClient_OK(EthernetClient* eth);

  //int operator==(const EthernetClient&);
  //int operator!=(const EthernetClient& rhs) { return !this->operator==(rhs); };

  // parsing methods

  void EthernetClient_setTimeout(EthernetClient* eth, unsigned long timeout);  // sets maximum milliseconds to wait for stream data, default is 1 second
  unsigned long EthernetClient_getTimeout(EthernetClient* eth);

  int EthernetClient_find_string(EthernetClient* eth, char *target);   // reads data from the stream until the target string is found
  int EthernetClient_find_byte(EthernetClient* eth, uint8_t *target);
  // returns 1 if target string is found, 0 if timed out (see setTimeout)

  int EthernetClient_find_string_length(EthernetClient* eth, char *target, size_t length);   // reads data from the stream until the target string of given length is found
  int EthernetClient_find_buf(EthernetClient* eth, uint8_t *target, size_t length);
  // returns 1 if target string is found, 0 if timed out

  int EthernetClient_find_char(EthernetClient* eth, char target);

  int EthernetClient_findUntil_string(EthernetClient* eth, char *target, char *terminator);   // as find_char but search ends if the terminator string is found
  int EthernetClient_findUntil_buf(EthernetClient* eth, uint8_t *target, char *terminator);

  int EthernetClient_findUntil_string_length(EthernetClient* eth, char *target, size_t targetLen, char *terminate, size_t termLen);   // as above but search ends if the terminate string is found
  int EthernetClient_findUntil_buf_length(EthernetClient* eth, uint8_t *target, size_t targetLen, char *terminate, size_t termLen);

  long EthernetClient_parseInt(EthernetClient* eth, LookaheadModefg lookahead, char ignore);
  // returns the first valid (long) integer value from the current position.
  // lookahead determines how parseInt_protected looks ahead in the stream.
  // See LookaheadMode enumeration at the top of the file.
  // Lookahead is terminated by the first character that is not a valid part of an integer.
  // Once parsing commences, 'ignore' will be skipped in the stream.

  float EthernetClient_parseFloat(EthernetClient* eth, LookaheadModefg lookahead,  char ignore);
  // float version of parseInt_protected

  size_t EthernetClient_readBytes(EthernetClient* eth, char *buffer, size_t length); // read_string chars from stream into buffer
  //size_t readBytes(uint8_t *buffer, size_t length) { return readBytes((char *)buffer, length); }
  // terminates if length characters have been read_string or timeout (see setTimeout)
  // returns the number of characters placed in the buffer (0 means no valid data found)

  size_t EthernetClient_readBytesUntil(EthernetClient* eth, char terminator, char *buffer, size_t length); // as readBytes with terminator character
  //size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length) { return readBytesUntil(terminator, (char *)buffer, length); }
  // terminates if length characters have been read_string, timeout, or if the terminator character  detected
  // returns the number of characters placed in the buffer (0 means no valid data found)

  GENERATE_PRINT_HEADER(EthernetClient);

  //extern PrintMethods  EthernetClientPrintMethods;

  //size_t EthernetClient_write_byte(EthernetClient* eth, uint8_t b);

  //size_t EthernetClient_write_string(EthernetClient* eth, const char *str);

  //size_t EthernetClient_write(void* eth, const char *buffer, size_t size);

  //size_t EthernetClient_print(EthernetClient* eth, const char[]);
  //size_t EthernetClient_print_char(EthernetClient* eth, char c);
  //size_t EthernetClient_print_uchar(EthernetClient* eth, unsigned char c, int = DEC);
  //size_t EthernetClient_print_int(EthernetClient* eth, int, int = DEC);
  //size_t EthernetClient_print_uint(EthernetClient* eth, unsigned int, int = DEC);
  //size_t EthernetClient_print_long(EthernetClient* eth, long, int = DEC);
  //size_t EthernetClient_print_ulong(EthernetClient* eth, unsigned long, int = DEC);
  //size_t EthernetClient_print_double(EthernetClient* eth, double, int = 2);

  //size_t EthernetClient_println(EthernetClient* eth, const char[]);
  //size_t EthernetClient_println_char(EthernetClient* eth, char);
  //size_t EthernetClient_println_uchar(EthernetClient* eth, unsigned char, int = DEC);
  //size_t EthernetClient_println_int(EthernetClient* eth, int, int = DEC);
  //size_t EthernetClient_println_uint(EthernetClient* eth, unsigned int, int = DEC);
  //size_t EthernetClient_println_long(EthernetClient* eth, long, int = DEC);
  //size_t EthernetClient_println_ulong(EthernetClient* eth, unsigned long, int = DEC);
  //size_t EthernetClient_println_double(EthernetClient* eth, double, int = 2);
  //size_t EthernetClient_print_newline(EthernetClient* eth);


	uint8_t* EthernetClient_rawIPAddress(EthernetClient* eth, IPAddress* addr);
	//long parseInt_protected(char ignore) { return parseInt(SKIP_ALLfg, ignore); }
	//float parseFloat(char ignore) { return parseFloat(SKIP_ALLfg, ignore); }
	// These overload exists for compatibility with any class that has derived
	// Stream and used parseFloat/Int with a custom ignore character. To keep
	// the public API simple, these overload remains protected.

	// This allows you to search for an arbitrary number of strings.
	// Returns index of the target that is found first or -1 if timeout occurs.
	int EthernetClient_findMulti(EthernetClient* eth, struct MultiTarget *targets, int tCount);

	int EthernetClient_timedRead(EthernetClient* eth);    // private method to read_string stream with timeout
	int EthernetClient_timedPeek(EthernetClient* eth);    // private method to peek stream with timeout
	int EthernetClient_peekNextDigit(EthernetClient* eth, LookaheadModefg lookahead, int detectDecimal); // returns the next numeric digit in the stream or -1 if timeout

#ifdef __cplusplus
}
#endif

#endif
