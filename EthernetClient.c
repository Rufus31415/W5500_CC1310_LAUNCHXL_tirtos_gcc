#include "W5500.h"
#include "socket.h"

//extern "C" {
  #include "string.h"
//}

#ifdef ARDUINO
#include "Arduino.h"
#else
#include "ArduinoCompatibility.h"
#endif
#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "Dns.h"

#ifdef __cplusplus
extern "C" {
#endif


	uint16_t EthernetClient_srcport = 1024;

	void EthernetClient_begin_default(EthernetClient* eth) {
		EthernetClient_begin(eth, MAX_SOCK_NUM);
	}

	void EthernetClient_begin(EthernetClient* eth, uint8_t sock) {
		eth->_sock = sock;
		eth->_timeout = 1000;
	}

	int EthernetClient_connect_host(EthernetClient* eth, const char* host, uint16_t port) {
		// Look up the host first
		int ret = 0;
		DNSClient dns;
		IPAddress remote_addr;

		DNSClient_begin(&dns, Ethernet_dnsServerIP());
		ret = DNSClient_getHostByName(&dns, host, &remote_addr);
		if (ret == 1) {
			return EthernetClient_connect_ip(eth, remote_addr, port);
		}
		else {
			return ret;
		}
	}

	int EthernetClient_connect_ip(EthernetClient* eth, IPAddress ip, uint16_t port) {
		if (eth->_sock != MAX_SOCK_NUM)
			return 0;

		for (int i = 0; i < MAX_SOCK_NUM; i++) {
			uint8_t s = W5500_readSnSR(i);
			if (s == SnSR_CLOSED || s == SnSR_FIN_WAIT || s == SnSR_CLOSE_WAIT) {
				eth->_sock = i;
				break;
			}
		}

		if (eth->_sock == MAX_SOCK_NUM)
			return 0;

		EthernetClient_srcport++;
		if (EthernetClient_srcport == 0) EthernetClient_srcport = 1024;
		socket(eth->_sock, SnMR_TCP, EthernetClient_srcport, 0);

		if (!connect(eth->_sock, EthernetClient_rawIPAddress(eth, &ip), port)) {
			eth->_sock = MAX_SOCK_NUM;
			return 0;
		}

		while (EthernetClient_status(eth) != SnSR_ESTABLISHED) {
		    delay(1);
			if (EthernetClient_status(eth) == SnSR_CLOSED) {
				eth->_sock = MAX_SOCK_NUM;
				return 0;
			}
		}

		return 1;
	}

	size_t EthernetClient_write(void* objeth, const uint8_t *buf, size_t size) {
		EthernetClient* eth = (EthernetClient*)objeth;
		if (eth->_sock == MAX_SOCK_NUM) {
			return 0;
		}
		if (!send(eth->_sock, buf, size)) {
			return 0;
		}
		return size;
	}

	int EthernetClient_available(EthernetClient* eth) {
		if (eth->_sock != MAX_SOCK_NUM)
			return W5500_getRXReceivedSize(eth->_sock);
		return 0;
	}

	int EthernetClient_read(EthernetClient* eth) {
		uint8_t b;
		if (recv(eth->_sock, &b, 1) > 0)
		{
			// recv worked
			return b;
		}
		else
		{
			// No data available
			return -1;
		}
	}

	int EthernetClient_read_buf(EthernetClient* eth, uint8_t *buf, size_t size) {
		return recv(eth->_sock, buf, size);
	}

	int EthernetClient_peek(EthernetClient* eth) {
		uint8_t b;
		// Unlike recv, peek doesn't check to see if there's any data available, so we must
		if (!EthernetClient_available(eth))
			return -1;
		peek(eth->_sock, &b);
		return b;
	}

	void EthernetClient_flush(EthernetClient* eth) {
		flush(eth->_sock);
	}

	void EthernetClient_stop(EthernetClient* eth) {
		if (eth->_sock == MAX_SOCK_NUM)
			return;

		// attempt to close the connection gracefully (send a FIN to other side)
		disconnect(eth->_sock);
		unsigned long start = millis();

		// wait a second for the connection to close
		while (EthernetClient_status(eth) != SnSR_CLOSED && millis() - start < 1000)
			delay(1);

		// if it hasn't closed, close it forcefully
		if (EthernetClient_status(eth) != SnSR_CLOSED)
			close(eth->_sock);

		Ethernet_server_port[eth->_sock] = 0;
		eth->_sock = MAX_SOCK_NUM;
	}

	uint8_t EthernetClient_connected(EthernetClient* eth) {
		if (eth->_sock == MAX_SOCK_NUM) return 0;

		uint8_t s = EthernetClient_status(eth);
		return !(s == SnSR_LISTEN || s == SnSR_CLOSED || s == SnSR_FIN_WAIT ||
			(s == SnSR_CLOSE_WAIT && !EthernetClient_available(eth)));
	}

	uint8_t EthernetClient_status(EthernetClient* eth) {
		if (eth->_sock == MAX_SOCK_NUM) return SnSR_CLOSED;
		return W5500_readSnSR(eth->_sock);
	}

	// the next function allows us to use the client returned by
	// EthernetServer::EthernetClient_available(eth) as the condition in an if-statement.

	int EthernetClient_OK(EthernetClient* eth) {
		return eth->_sock != MAX_SOCK_NUM;
	}

	//int EthernetClient_operator==(const EthernetClient& rhs) {
	//  return eth->_sock == rhs.eth->_sock && eth->_sock != MAX_SOCK_NUM && rhs.eth->_sock != MAX_SOCK_NUM;
	//}

// private method to read_string stream with timeout
	int EthernetClient_timedRead(EthernetClient* eth)
	{
		int c;
		eth->_startMillis = millis();
		do {
			c = EthernetClient_read(eth);
			if (c >= 0) return c;
		} while (millis() - eth->_startMillis < eth->_timeout);
		return -1;     // -1 indicates timeout
	}

	// private method to peek stream with timeout
	int EthernetClient_timedPeek(EthernetClient* eth)
	{
		int c;
		eth->_startMillis = millis();
		do {
			c = EthernetClient_peek(eth);
			if (c >= 0) return c;
		} while (millis() - eth->_startMillis < eth->_timeout);
		return -1;     // -1 indicates timeout
	}

	// returns peek of the next digit in the stream or -1 if timeout
	// discards non-numeric characters
	int EthernetClient_peekNextDigit(EthernetClient* eth, LookaheadModefg lookahead, int detectDecimal)
	{
		int c;
		while (1) {
			c = EthernetClient_timedPeek(eth);

			if (c < 0 ||
				c == '-' ||
				(c >= '0' && c <= '9') ||
				(detectDecimal && c == '.')) return c;

			switch (lookahead) {
			case SKIP_NONEfg: return -1; // Fail code.
			case SKIP_WHITESPACEfg:
				switch (c) {
				case ' ':
				case '\t':
				case '\r':
				case '\n': break;
				default: return -1; // Fail code.
				}
			case SKIP_ALLfg:
				break;
			}
			EthernetClient_read(eth);  // discard non-numeric
		}
	}

	// Public Methods
	//////////////////////////////////////////////////////////////

	void EthernetClient_setTimeout(EthernetClient* eth, unsigned long timeout)  // sets the maximum number of milliseconds to wait
	{
		eth->_timeout = timeout;
	}

	// find_char returns 1 if the target string is found
	int  EthernetClient_find_string(EthernetClient* eth, char *target)
	{
		return EthernetClient_findUntil_string_length(eth, target, strlen(target), NULL, 0);
	}

	// reads data from the stream until the target string of given length is found
	// returns 1 if target string is found, 0 if timed out
	int EthernetClient_find_string_length(EthernetClient* eth, char *target, size_t length)
	{
		return EthernetClient_findUntil_string_length(eth, target, length, NULL, 0);
	}

	// as find_char but search ends if the terminator string is found
	int  EthernetClient_findUntil_string(EthernetClient* eth, char *target, char *terminator)
	{
		return EthernetClient_findUntil_string_length(eth, target, strlen(target), terminator, strlen(terminator));
	}

	// reads data from the stream until the target string of the given length is found
	// search terminated if the terminator string is found
	// returns 1 if target string is found, 0 if terminated or timed out
	int EthernetClient_findUntil_string_length(EthernetClient* eth, char *target, size_t targetLen, char *terminator, size_t termLen)
	{
		if (terminator == NULL) {
			MultiTarget t[1] = { { target, targetLen, 0 } };
			return EthernetClient_findMulti(eth, t, 1) == 0 ? 1 : 0;
		}
		else {
			MultiTarget t[2] = { { target, targetLen, 0 },{ terminator, termLen, 0 } };
			return EthernetClient_findMulti(eth, t, 2) == 0 ? 1 : 0;
		}
	}

	// returns the first valid (long) integer value from the current position.
	// lookahead determines how parseInt_protected looks ahead in the stream.
	// See LookaheadMode enumeration at the top of the file.
	// Lookahead is terminated by the first character that is not a valid part of an integer.
	// Once parsing commences, 'ignore' will be skipped in the stream.
	long EthernetClient_parseInt(EthernetClient* eth, LookaheadModefg lookahead, char ignore)
	{
		int isNegative = 0;
		long value = 0;
		int c;

		c = EthernetClient_peekNextDigit(eth, lookahead, 0);
		// ignore non numeric leading characters
		if (c < 0)
			return 0; // zero returned if timeout

		do {
			if (c == ignore)
				; // ignore this character
			else if (c == '-')
				isNegative = 1;
			else if (c >= '0' && c <= '9')        // is c a digit?
				value = value * 10 + c - '0';
			EthernetClient_read(eth);  // consume the character we got with peek
			c = EthernetClient_timedPeek(eth);
		} while ((c >= '0' && c <= '9') || c == ignore);

		if (isNegative)
			value = -value;
		return value;
	}

	// as parseInt_protected but returns a floating point value
	float EthernetClient_parseFloat(EthernetClient* eth, LookaheadModefg lookahead, char ignore)
	{
		int isNegative = 0;
		int isFraction = 0;
		long value = 0;
		int c;
		float fraction = 1.0;

		c = EthernetClient_peekNextDigit(eth, lookahead, 1);
		// ignore non numeric leading characters
		if (c < 0)
			return 0; // zero returned if timeout

		do {
			if (c == ignore)
				; // ignore
			else if (c == '-')
				isNegative = 1;
			else if (c == '.')
				isFraction = 1;
			else if (c >= '0' && c <= '9') {      // is c a digit?
				value = value * 10 + c - '0';
				if (isFraction)
					fraction *= 0.1;
			}
			EthernetClient_read(eth);  // consume the character we got with peek
			c = EthernetClient_timedPeek(eth);
		} while ((c >= '0' && c <= '9') || (c == '.' && !isFraction) || c == ignore);

		if (isNegative)
			value = -value;
		if (isFraction)
			return value * fraction;
		else
			return value;
	}

	// read_string characters from stream into buffer
	// terminates if length characters have been read_string, or timeout (see setTimeout)
	// returns the number of characters placed in the buffer
	// the buffer is NOT null terminated.
	//
	size_t EthernetClient_readBytes(EthernetClient* eth, char *buffer, size_t length)
	{
		size_t count = 0;
		while (count < length) {
			int c = EthernetClient_timedRead(eth);
			if (c < 0) break;
			*buffer++ = (char)c;
			count++;
		}
		return count;
	}


	// as readBytes with terminator character
	// terminates if length characters have been read_string, timeout, or if the terminator character  detected
	// returns the number of characters placed in the buffer (0 means no valid data found)

	size_t EthernetClient_readBytesUntil(EthernetClient* eth, char terminator, char *buffer, size_t length)
	{
		if (length < 1) return 0;
		size_t index = 0;
		while (index < length) {
			int c = EthernetClient_timedRead(eth);
			if (c < 0 || c == terminator) break;
			*buffer++ = (char)c;
			index++;
		}
		return index; // return number of characters, not including null terminator
	}

	int EthernetClient_findMulti(EthernetClient* eth, MultiTarget *targets, int tCount) {
		// any zero length target string automatically matches and would make
		// a mess of the rest of the algorithm.
		for (struct MultiTarget *t = targets; t < targets + tCount; ++t) {
			if (t->len <= 0)
				return t - targets;
		}

		while (1) {
			int c = EthernetClient_timedRead(eth);
			if (c < 0)
				return -1;

			for (struct MultiTarget *t = targets; t < targets + tCount; ++t) {
				// the simple case is if we match, deal with that first.
				if (c == t->str[t->index]) {
					if (++t->index == t->len)
						return t - targets;
					else
						continue;
				}

				// if not we need to walk back and see if we could have matched further
				// down the stream (ie '1112' doesn't match the first position in '11112'
				// but it will match the second position so we can't just reset the current
				// index to 0 when we find_char a mismatch.
				if (t->index == 0)
					continue;

				int origIndex = t->index;
				do {
					--t->index;
					// first check if current char works against the new current index
					if (c != t->str[t->index])
						continue;

					// if it's the only char then we're good, nothing more to check
					if (t->index == 0) {
						t->index++;
						break;
					}

					// otherwise we need to check the rest of the found string
					int diff = origIndex - t->index;
					size_t i;
					for (i = 0; i < t->index; ++i) {
						if (t->str[i] != t->str[i + diff])
							break;
					}

					// if we successfully got through the previous loop then our current
					// index is good.
					if (i == t->index) {
						t->index++;
						break;
					}

					// otherwise we just try the next index
				} while (t->index);
			}
		}
		// unreachable
		return -1;
	}


	int EthernetClient_find_byte(EthernetClient* eth, uint8_t *target) { return EthernetClient_find_string(eth, (char *)target); }
	// returns 1 if target string is found, 0 if timed out (see setTimeout)

	int EthernetClient_find_buf(EthernetClient* eth, uint8_t *target, size_t length) { return EthernetClient_find_string_length(eth, (char *)target, length); }
	// returns 1 if target string is found, 0 if timed out

	int EthernetClient_find_char(EthernetClient* eth, char target) { return EthernetClient_find_string_length(eth, &target, 1); }

	int EthernetClient_findUntil_buf(EthernetClient* eth, uint8_t *target, char *terminator) { return EthernetClient_findUntil_string(eth, (char *)target, terminator); }

	int EthernetClient_findUntil_buf_length(EthernetClient* eth, uint8_t *target, size_t targetLen, char *terminate, size_t termLen) { return EthernetClient_findUntil_string_length(eth, (char *)target, targetLen, terminate, termLen); }

	uint8_t* EthernetClient_rawIPAddress(EthernetClient* eth, IPAddress* addr) { return addr->bytes; };

	unsigned long EthernetClient_getTimeout(EthernetClient* eth) { return eth->_timeout; }

	GENERATE_PRINT_INSTANCE(EthernetClient)

#ifdef __cplusplus
}
#endif
