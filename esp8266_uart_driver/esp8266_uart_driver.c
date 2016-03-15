/*
 * esp8266_uart_driver.c
 *
 * Created on: 2 févr. 2016
 * @author: Zekemyapp
 */


#include "esp8266_uart_driver.h"

const char AT[] = "AT\r\n";
const char RST[] = "AT+RST\r\n";
const char JAP[] = "AT+CWJAP=\"%s\",\"%s\"\r\n";
const char QAP[] = "AT+CWQAP\r\n";
const char CIFSR[] = "AT+CIFSR\r\n";
const char CWSAP[] = "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n";
const char CWMODE[] = "AT+CWMODE=%d\r\n";
const char CIPSTART[] = "AT+CIPSTART=%d,\"%s\",\"%s\",%d\r\n";
const char CIPMUX[] = "AT+CIPMUX=%d\r\n";
const char CIPSTATUS[] = "AT+CIPSTATUS\r\n";
const char CIPSEND[] = "AT+CIPSEND=%d,%d\r\n";
const char CIPCLOSE[] = "AT+CIPCLOSE=%d\r\n";
const char CIPSERVER[] = "AT+CIPSERVER=%d,%d\r\n";
const char CIPSTO[] = "AT+CIPSTO=%d\r\n";
const char CIOBAUD[] = "AT+CIOBAUD=%d\r\n";

static char txbuffer[TX_BUFFER_SIZE];
static char rxbuffer[RX_BUFFER_SIZE];

/*
 * AT Check
 *
 * @Sends 		AT
 * @Reponse		OK
 */
void ESP8266_UART_AT(){
	UART_DRV_SendData(ESP_UART_INSTANCE, AT, sizeof(AT)-1);
}

/*
 * AT Reset
 *
 * Restart the device
 *
 * @sends		AT+RST
 * @Reponse 	OK
 */
void ESP8266_UART_RST(){
	UART_DRV_SendData(ESP_UART_INSTANCE, RST, sizeof(RST)-1);
}

/*
 * Join Access Point
 *
 * @sends		AT+CWJAP="SSID","Password"
 * @Reponse		OK
 */
bool ESP8266_UART_JAP(const char* ssid,const char* password){
	uart_status_t res;

	sprintf(txbuffer,JAP,ssid,password);
	res = UART_DRV_SendDataBlocking(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer),100);

	do{
		res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 100);
	} while (*rxbuffer != '\n');

	res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 33, 20000);

	rxbuffer[33]='\0';

	if(res !=  kStatus_UART_Success) return true;
	else{
		return (strcmp(rxbuffer,"WIFI CONNECTED\r\nWIFI GOT IP\r\n\r\nOK") ? true : false);
	}
}

/*
 * Quit Access Point
 *
 * @sends		AT+CWQAP
 * @Reponse		OK
 */
bool ESP8266_UART_QAP(){
	uart_status_t res;
	res = UART_DRV_SendDataBlocking(ESP_UART_INSTANCE, QAP, sizeof(QAP)-1,100);

	if(res !=  kStatus_UART_Success) return true;

	// Receive the print of the request done
	do{
		res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 100);
	} while (*rxbuffer != '\n');

	// Receive the response
	res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 4, 100);
	rxbuffer[4]='\0';

	if(res !=  kStatus_UART_Success) return true;
	else{
		if (!strcmp(rxbuffer,"\r\nOK")){
			// If the Wi-Fi was connected is going to receive extra data
			res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 19, 100);
			return false;

		} else return true;
	}
}

/*
 * Get IP Address
 *
 * @sends		AT+CIFSR
 * @Reponse		AT+CIFSR 192.168.0.105
 * 				OK
 */
void ESP8266_UART_IP(){
	UART_DRV_SendData(ESP_UART_INSTANCE, CIFSR, sizeof(CIFSR)-1);
}

/*
 * Set Parameters of Access Point
 *
 * @sends		AT+ CWSAP= <ssid>,<pwd>,<chl>, <ecn>
 * @Reponse		Query
 * 				ssid, pwd
 * 				chl = channel, ecn = encryption
 * 				0 - Open. No security.
 * 				1 - WEP.
 * 				2 - WPA_PSK
 * 				3 - WPA2_PSK
 * 				4 - WPA_WPA2_PSK
 */
void ESP8266_UART_SAP(const char* ssid, const char* password, kwifi_channel_t channel, kwifi_security_t encryption){
	sprintf(txbuffer,CWSAP,ssid,password,channel,encryption);
	UART_DRV_SendData(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer));
}

/*
 * Wi-Fi Mode
 *
 * @sends		AT+CWMODE=?
 * @Reponse		Query
 * 				STA
 * 				AP
 * 				BOTH
 */
void ESP8266_UART_MODE(kwifi_mode_t mode){
	sprintf(txbuffer,CWMODE,mode);
	UART_DRV_SendData(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer));
}

/*
 * Set up TCP or UDP connection
 *
 * @sends		AT+CIPSTART= <id><type>,<addr>, <port>
 * @reponse		Query
 * 				id = 0-4, type = TCP/UDP, addr = IP address, port= port
 */
bool ESP8266_UART_IPSTART(kwifi_socket_t socket, kwifi_conntype_t type, const char* address, uint16_t port){
	uart_status_t res;

	// Empty the rx buffer to be sure there are not rests of previous operations
	do{res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 100);}
	while (res == kStatus_UART_Success);

	// Sends the command
	sprintf(txbuffer,CIPSTART,socket,type == kwifi_TCP ? "TCP" : "UDP", address, port );
	res = UART_DRV_SendDataBlocking(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer),100);
	if(res !=  kStatus_UART_Success) return true;

	// Receive a copy of the command and ditch it
	do{res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 1000);}
	while (*rxbuffer != '\n');

	// Receive the 7 first characters of the answer
	res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 7, 10000);
	rxbuffer[7]='\0';

	// Check the answer
	if(res !=  kStatus_UART_Success) return true;
	else return ((strcmp(rxbuffer,"ALREADY") && strcmp(rxbuffer,"0,CONNE")) ? true : false);
}

/*
 * TCP/UDP Number of Connections
 *
 * @sends		AT+ CIPMUX=0
 * 				AT+ CIPMUX=1
 * @reponse		Query
 * 				Single
 * 				Multiple
 */
bool ESP8266_UART_IPMUX(kwifi_connmux_t mux){
	uart_status_t res;

	sprintf(txbuffer,CIPMUX,mux );
	res = UART_DRV_SendDataBlocking(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer),100);

	if(res !=  kStatus_UART_Success) return true;

	do{
		res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 100);
	} while (*rxbuffer != '\n');

	res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 4, 100);

	rxbuffer[4]='\0';

	if(res !=  kStatus_UART_Success) return true;
	else{
		return (strcmp(rxbuffer,"\r\nOK") ? true : false);
	}

}

/*
 * TCP/IP Connection Status
 *
 * @Sends		AT+CIPSTATUS
 * @Reponse
 */
void ESP8266_UART_IPSTATUS(){
	UART_DRV_SendData(ESP_UART_INSTANCE, CIPSTATUS, strlen(CIPSTATUS));
}

/*
 * Send TCP/IP data
 *
 * @Sends		(CIPMUX=0) AT+CIPSEND=<length>
 * 				(CIPMUX=1) AT+CIPSEND= <id>,<length>
 */
bool ESP8266_UART_IPSEND_HEADER(kwifi_socket_t socket, uint16_t length){
	uart_status_t res;

	// Empty the rx buffer to be sure there are not rests of previous operations
	do{res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 100);}
	while (res == kStatus_UART_Success);

	// Sends the request
	sprintf(txbuffer,CIPSEND,socket,length);
	res = UART_DRV_SendDataBlocking(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer),100);
	if(res !=  kStatus_UART_Success) return true;

	// Receive copy of the request sent and ditch it
	do{res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 100);}
	while (*rxbuffer != '\n');

	// Receive the answer
	res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 4, 1000);
	rxbuffer[4]='\0';

	// Check the answer for success or link not valid
	if(res !=  kStatus_UART_Success) return true;
	else return (strcmp(rxbuffer,"\r\nOK") ? true : false);
}


uint16_t ESP8266_UART_IPSEND_BODY(uint8_t* body, uint8_t* buffer, uint16_t size){
	uart_status_t res;

	bool found_1st = false;
	bool found_2nd = false;
	bool found_ready = false;
	bool check_for_more = false;
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t offset = 0;
	char tmp_buf[16] = {0};

	// Empty the rx buffer to be sure there are not rests of previous operations
	do{res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 100);}
	while (res == kStatus_UART_Success);

	// Sends the data containing the body of the http request
	res = UART_DRV_SendDataBlocking(ESP_UART_INSTANCE, body, strlen(body),1000);
	if(res !=  kStatus_UART_Success) return 0;

	// Handle the received response from the server
	for(;;){
		res = UART_DRV_ReceiveDataBlocking(ESP_UART_INSTANCE, rxbuffer, 1, 2000);
		if(res !=  kStatus_UART_Success) break;

		if(check_for_more){
			if(*rxbuffer >= '0' && *rxbuffer <= '5' ){
				buffer[x] = '\0';
				break;

			} else {
				check_for_more = false;
				found_ready = false;
				found_2nd = false;
				offset = x;
				y = 0;
				continue;
			}
		}

		if(found_ready){
			buffer[x] = *rxbuffer;
			x+=1;

			if(x < y) continue;
			else{
				check_for_more = true;
				continue;
			}
		}

		if(found_2nd){
			if(*rxbuffer == ':'){
				uint16_t size_descriptor = y;
				uint8_t count;

				// Decodes the number of chars to receive
				y = 0;
				for(count=0;count<size_descriptor-6;count++){
					y*=10;
					y+=tmp_buf[count+6]-'0';
				}

				y += offset;
				if (y+1>size) return 0;

				found_ready = true;
				continue;
			}

			tmp_buf[y] = *rxbuffer;
			y+=1;

			continue;
		}

		if(found_1st && *rxbuffer == '+') found_2nd = true;
		else if(*rxbuffer == '+') found_1st = true;

	}

	return x;

}


/*
 * Close TCP / UDP connection
 *
 * @Sends		AT+CIPCLOSE=<id> or AT+CIPCLOSE
 */
void ESP8266_UART_IPCLOSE(kwifi_socket_t socket){
	sprintf(txbuffer,CIPCLOSE,socket);
	UART_DRV_SendData(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer));
}

/*
 * Set as server
 *
 * @Sends		AT+ CIPSERVER= <mode>[,<port>]
 * @Reponse		mode 0 to close server mode; mode 1 to open; port = port
 *
 */
void ESP8266_UART_IPSERVER(kwifi_socket_t socket, uint16_t port){
	char txbuffer[100];
	sprintf(txbuffer,CIPSERVER,socket,port);
	UART_DRV_SendData(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer));
}

/*
 * Set the server timeout
 *
 * @Sends		AT+CIPSTO=<time>
 * @Reponse		Query
 * 				<time>0~28800 in seconds
 */
void ESP8266_UART_IPSTOP(uint16_t time){
	char txbuffer[100];
	sprintf(txbuffer,CIPSTO,time);
	UART_DRV_SendData(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer));
}

/*
 * Baud Rate
 *
 * @Sends		AT+CIOBAUD?
 * 				Supported: 9600, 19200, 38400, 74880, 115200, 230400, 460800, 921600
 * @Reponse		Query AT+CIOBAUD? +CIOBAUD:9600 OK
 */
void ESP8266_UART_BAUD(int16_t baud){
	char txbuffer[100];
	sprintf(txbuffer,CIOBAUD,baud);
	UART_DRV_SendData(ESP_UART_INSTANCE, txbuffer, strlen(txbuffer));
}

