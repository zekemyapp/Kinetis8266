/*
 * esp8266_uart_driver.h
 *
 *  Created on: 2 févr. 2016
 *      Author: Juan
 */

#ifndef ESP8266_UART_DRIVER_ESP8266_UART_DRIVER_H_
#define ESP8266_UART_DRIVER_ESP8266_UART_DRIVER_H_

#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "stdbool.h"

#include "board.h"
#include "fsl_uart_driver.h"

/******************
 * CONFIGURATION
 ******************/

#define ESP_UART_INSTANCE BOARD_MODULE1_UART_INSTANCE
#define TX_BUFFER_SIZE 250
#define RX_BUFFER_SIZE 250

/*
 * Access Points channels
 */
typedef enum {
	kwifi_channel1 = 1,
	kwifi_channel2,
	kwifi_channel3,
	kwifi_channel4,
	kwifi_channel5,
	kwifi_channel6,
	kwifi_channel7,
	kwifi_channel8,
	kwifi_channel9,
	kwifi_channel10,
	kwifi_channel11,
	kwifi_channel12,
	kwifi_channel13,
	kwifi_channel14

} kwifi_channel_t;

/*
 * Types of 802.11 security
 */
typedef enum {
	kwifi_sec_disabled = 0,
	kwifi_sec_wep,
	kwifi_sec_wpapsk,
	kwifi_sec_wpa2psk,
	kwifi_sec_wpa_wpa2psk

} kwifi_security_t;

/*
 * Module Mode
 */
typedef enum {
	kwifi_station = 1,
	kwifi_accespoint,
	kwifi_both

} kwifi_mode_t;

/*
 * Query type
 */
typedef enum {
	kwifi_TCP = 0,
	kwifi_UDP
} kwifi_conntype_t;

/*
 * Connection Socket
 */
typedef enum {
	kwifi_socket0 = 0,
	kwifi_socket1,
	kwifi_socket2,
	kwifi_socket3,
	kwifi_socket4

} kwifi_socket_t;

/*
 * Connections Multiplexer
 */
typedef enum {
	kwifi_single=0,
	kwifi_multiple
} kwifi_connmux_t;


/*
 * AT Check
 *
 * @Sends 		AT
 * @Reponse		OK
 */
void ESP8266_UART_AT();

/*
 * AT Reset
 *
 * Restart the device
 *
 * @sends		AT+RST
 * @Reponse 	OK
 */
void ESP8266_UART_RST();

/*
 * Join Access Point
 *
 * @sends		AT+CWJAP="SSID","Password"
 * @Reponse		OK
 */
bool ESP8266_UART_JAP(const char* ssid,const char* password);

/*
 * Quit Access Point
 *
 * @sends		AT+CWQAP
 * @Reponse		OK
 */
bool ESP8266_UART_QAP();

/*
 * Get IP Address
 *
 * @sends		AT+CIFSR
 * @Reponse		AT+CIFSR 192.168.0.105
 * 				OK
 */
void ESP8266_UART_IP();

/*
 * Set Parameters of Access Point
 *
 * @sends		AT+ CWSAP= <ssid>,<pwd>,<chl>, <ecn>
 * @Reponse		Query
 * 				ssid, pwd
 * 				chl = channel, ecn = encryption
 */
void ESP8266_UART_SAP(const char* ssid, const char* password, kwifi_channel_t channel, kwifi_security_t encryption);

/*
 * Wi-Fi Mode
 *
 * @sends		AT+CWMODE=?
 * @Reponse		Query
 * 				STA
 * 				AP
 * 				BOTH
 */
void ESP8266_UART_MODE(kwifi_mode_t mode);

/*
 * Set up TCP or UDP connection
 *
 * @sends		AT+CIPSTART= <id><type>,<addr>, <port>
 * @reponse		Query
 * 				id = 0-4, type = TCP/UDP, addr = IP address, port= port
 */
bool ESP8266_UART_IPSTART(kwifi_socket_t socket, kwifi_conntype_t type, const char* address, uint16_t port);

/*
 * TCP/UDP Number of Connections
 *
 * @sends		AT+ CIPMUX=0
 * 				AT+ CIPMUX=1
 * @reponse		Query
 * 				Single
 * 				Multiple
 */
bool ESP8266_UART_IPMUX(kwifi_connmux_t mux);

/*
 * TCP/IP Connection Status
 *
 * @Sends		AT+CIPSTATUS
 * @Reponse
 */
void ESP8266_UART_IPSTATUS();

/*
 * Send TCP/IP data
 *
 * @Sends		(CIPMUX=0) AT+CIPSEND=<length>
 * 				(CIPMUX=1) AT+CIPSEND= <id>,<length>
 */
bool ESP8266_UART_IPSEND_HEADER(kwifi_socket_t socket, uint16_t length);


uint16_t ESP8266_UART_IPSEND_BODY(const uint8_t* body, char* buffer, uint16_t size);

/*
 * Close TCP / UDP connection
 *
 * @Sends		AT+CIPCLOSE=<id> or AT+CIPCLOSE
 */
void ESP8266_UART_IPCLOSE(kwifi_socket_t socket);

/*
 * Set as server
 *
 * @Sends		AT+ CIPSERVER= <mode>[,<port>]
 * @Reponse		mode 0 to close server mode; mode 1 to open; port = port
 */
void ESP8266_UART_IPSERVER(kwifi_socket_t socket, uint16_t port);

/*
 * Set the server timeout
 *
 * @Sends		AT+CIPSTO=<time>
 * @Reponse		Query
 * 				<time>0~28800 in seconds
 */
void ESP8266_UART_IPSTOP(uint16_t time);

/*
 * Baud Rate
 *
 * @Sends		AT+CIOBAUD?
 * 				Supported: 9600, 19200, 38400, 74880, 115200, 230400, 460800, 921600
 * @Reponse		Query AT+CIOBAUD? +CIOBAUD:9600 OK
 */
void ESP8266_UART_BAUD(int16_t baud);

#endif /* ESP8266_UART_DRIVER_ESP8266_UART_DRIVER_H_ */
