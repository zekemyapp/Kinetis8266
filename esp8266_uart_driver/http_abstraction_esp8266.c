/*
 * http_abstraction.h
 *
 * Created on: 1 mars 2016
 * Author: @author: Zekemyapp
 *
 * This library is created to abstract the ESP Driver specific command using
 * a more generalized calls that can then be used by other libs
 */

#include "http_abstraction_esp8266.h"

static http_request_t* sockets[HTTP_SOCKET_NUMBER] = {0};

/*******************
 * STATIC FUNCTIONS
 *******************/

static kwifi_socket_t request_socket (void){
	kwifi_socket_t count = 0;
	while (count < HTTP_SOCKET_NUMBER && sockets[count] != 0) count+=1;
	return count;
}

/*
 * HTTP Request
 * Performs a request specified by the request structure
 *
 * @request Request structure see @typedef http_request_t
 * @buffer 	a buffer where the answer will be stored
 * @size	Size of the buffer
 * @returns	error code
 */
http_error_t HTTP_Request (http_request_t* request, uint8_t* buffer, uint16_t size){
	kwifi_socket_t socket;
	uint8_t loop_count;

	if( (socket=request_socket()) >= HTTP_SOCKET_NUMBER) return httpBussy;
	sockets[socket] = request;

	char* type;
	switch(request->type){
	case httpGet:
		type = "GET";
		break;
	case httpPost:
		type = "POST";
		break;
	case httpPut:
		type = "PUT";
		break;
	case httpDelete:
		type = "DELETE";
		break;
	}

	char payload[MAX_PAYLOAD_SIZE];
	sprintf(payload,"%s %s HTTP/1.0\r\n"
					"%s"
					"Content-Length: %u\r\n"
					"\r\n"
					"%s",type, request->path,request->header,strlen(request->body),request->body);

	loop_count = 0;
	while(ESP8266_UART_IPSTART(socket, kwifi_TCP, request->host, request->port) && loop_count < MAX_RETRY) loop_count +=1;
	if (loop_count >= MAX_RETRY){
		sockets[socket] = 0;
		return httpRequestError;
	}

	loop_count = 0;
	while(ESP8266_UART_IPSEND_HEADER(socket, strlen(payload)) && loop_count < MAX_RETRY) loop_count +=1;
	if (loop_count >= MAX_RETRY){
		sockets[socket] = 0;
		return httpRequestError;
	}

	uint16_t size_test;
	size_test = ESP8266_UART_IPSEND_BODY(payload,buffer,size);

	sockets[socket] = 0;

	if(size_test == 0) return httpRequestError;
	return httpSuccess;
}
