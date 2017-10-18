// 
// 
// 

#include "Ethernet.h"
#include "IPAddress.h"
#include "UnitTest.h"
#include "EthernetServer.h"
#include "Serial.h"
#include "EthernetBonjour.h"

#ifdef ARDUINO
#include "Arduino.h"
#else
#include "ArduinoCompatibility.h"
#endif

//#define TEST_CLIENT   1
//#define TEST_BONJOUR   1


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

									 // Set the static IP address to use if the DHCP fails to assign
IPAddress ip = {{ 192, 168, 0, 177 }};

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect_ip to (port 80 is default for HTTP):
EthernetClient client_ip;


// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer localServer;

int pageLoaded = 0;


void nameFound(const char* name, IPAddress ip)
{
	if (0 != ip.dword) {
		Serial_print("The IP address for '");
		Serial_print(name);
		Serial_print("' is ");


		char ipstr[17];

		IPAddress_toString(ip, ipstr);
		//sprintf(ipstr, "%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
		Serial_println(ipstr);
	}
	else {
		Serial_print("Resolving '");
		Serial_print(name);
		Serial_println("' timed out.");
	}
}

void InitTests() {
	// Open serial communications and wait for port to open:
	Serial_begin();

	Serial_println("UART OK");

	// start the Ethernet connection:
	if (Ethernet_begin_mac(mac) == 0) {
		//Serial_println("Failed to configure Ethernet using DHCP");
		// try to congifure using IP address instead of DHCP:
		Ethernet_begin_mac_ip(mac, ip);
	}

	// give the Ethernet shield a second to initialize:
	delay(100);


#ifdef TEST_CLIENT
	Serial_println("connecting...");

	IPAddress googleIP = { 216,58,205,3 };

	EthernetClient_begin_default(&client_ip);

	// if you get a connection, report back via serial:
	//if (EthernetClient_connect_host(&client, server, 80)) {
	if (EthernetClient_connect_ip(&client_ip, googleIP, 80)) {
		Serial_println("connected to google host");
		// Make a HTTP request:
		EthernetClient_println(&client_ip, "GET /search?q=arduinooo HTTP/1.1");
		EthernetClient_println(&client_ip, "Host: www.google.com");
		EthernetClient_println(&client_ip, "Connection: close");
		EthernetClient_print_newline(&client_ip);
	}
	else {
		// if you didn't get a connection to the server:
		Serial_println("fuck, connection failed");
	}
#endif

	EthernetServer_begin(&localServer, 80);


	Serial_print("server is at ");
	char ipStr[16];
	IPAddress_toString(Ethernet_localIP(), ipStr);
	Serial_println(ipStr);

#ifdef TEST_BONJOUR
	delay(100);
    int result = EthernetBonjour_begin("delta");
    if (result != 1) {
        Serial_println("Ethernet Bonjour init failed");
    }
    EthernetBonjour_setNameResolvedCallback(nameFound);
#endif

}

void LoopTests() {


#ifdef TEST_CLIENT
	if (!pageLoaded) {
		// if there are incoming bytes available
		// from the server, read_string them and print them:
		if (EthernetClient_available(&client_ip)) {
			char c = EthernetClient_read(&client_ip);
			Serial_write(c);
		}

		// if the server's disconnected, stop the client:
		if (!EthernetClient_connected(&client_ip)) {
			Serial_println("");
			Serial_println("disconnecting.");
			EthernetClient_stop(&client_ip);

			pageLoaded = 1;
		}
	}
#endif


	EthernetClient connectedClient = EthernetServer_available(&localServer);
	if (EthernetClient_OK(&connectedClient)) {
		Serial_println("new client");
		// an http request ends with a blank line
		int currentLineIsBlank = 1;
		while (EthernetClient_connected(&connectedClient)) {
			if (EthernetClient_available(&connectedClient)) {
				char c = EthernetClient_read(&connectedClient);
				Serial_write(c);
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank) {
					// send a standard http response header
					EthernetClient_println(&connectedClient, "HTTP/1.1 200 OK");
					EthernetClient_println(&connectedClient, "Content-Type: text/html");
					EthernetClient_println(&connectedClient, "Connection: close");  // the connection will be closed after completion of the response
					EthernetClient_println(&connectedClient, "Refresh: 10");  // refresh the page automatically every 5 sec
					EthernetClient_print_newline(&connectedClient);
					EthernetClient_println(&connectedClient, "<!DOCTYPE HTML>");
					EthernetClient_println(&connectedClient, "<html>");

					EthernetClient_print(&connectedClient, "</b>Coucou</b><br>");
					EthernetClient_println(&connectedClient, "<i>Test avec retour chariot :</i>");

					EthernetClient_print(&connectedClient, "<br>Char : ");
					EthernetClient_println_char(&connectedClient, 'A');

					EthernetClient_print(&connectedClient, "<br>Int : ");
					EthernetClient_println_int(&connectedClient, 42);

					EthernetClient_print(&connectedClient, "<br>Double : ");
					EthernetClient_println_double(&connectedClient, -15.32);

					EthernetClient_print(&connectedClient, "<br>Long : ");
					EthernetClient_println_long(&connectedClient, 100000);


					EthernetClient_println(&connectedClient, "<br><br><i>Test sans retour chariot :</i>");

					EthernetClient_print(&connectedClient, "<br>Char : ");
					EthernetClient_print_char(&connectedClient, 'A');

					EthernetClient_print(&connectedClient, "<br>Int : ");
					EthernetClient_print_int(&connectedClient, 42);

					EthernetClient_print(&connectedClient, "<br>Double : ");
					EthernetClient_print_double(&connectedClient, -15.32);

					EthernetClient_print(&connectedClient, "<br>Long : ");
					EthernetClient_print_long(&connectedClient, 100000);

					break;
				}
				if (c == '\n') {
					// you're starting a new line
					currentLineIsBlank = 1;
				}
				else if (c != '\r') {
					// you've gotten a character on the current line
					currentLineIsBlank = 0;
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);
		// close the connection:
		EthernetClient_stop(&connectedClient);
		Serial_println("client disconnected");


	}

#ifdef TEST_BONJOUR
	static int triggerResolve = 0;

	char* hostName = "DESKTOP-U1C3C2R";
	if (triggerResolve && !EthernetBonjour_isResolvingName()) {
	    triggerResolve = 0;

			Serial_print("Resolving '");
			Serial_print(hostName);
			Serial_println("' via Multicast DNS (Bonjour)...");

			// Now we tell the Bonjour library to resolve the host name. We give it a
			// timeout of 5 seconds (e.g. 5000 milliseconds) to find an answer. The
			// library will automatically resend the query every second until it
			// either receives an answer or your timeout is reached - In either case,
			// the callback function you specified in setup() will be called.

                if(!EthernetBonjour_resolveName(hostName, 5000)){
                    Serial_println("Failed to start resolving name");
                }
//		}
	}


	EthernetBonjour_run();
#endif

}
