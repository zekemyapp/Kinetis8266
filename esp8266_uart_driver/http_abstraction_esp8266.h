/*
 * http_abstraction.h
 *
 * Created on: 1 mars 2016
 * @author: Zekemyapp
 *
 * This library is created to abstract the ESP Driver specific command using
 * a more generalized calls that can then be used by other libs
 */

#ifndef SOURCES_LIBS_ESP8266_UART_DRIVER_HTTP_ABSTRACTION_ESP8266_H_
#define SOURCES_LIBS_ESP8266_UART_DRIVER_HTTP_ABSTRACTION_ESP8266_H_



/***************
 * INCLUDES
 ***************/

#include "stdbool.h"
#include "stdint.h"
#include "string.h"

#include "esp8266_uart_driver.h"

/**************
 * DEFINITIONS
 **************/

#define HTTP_SOCKET_NUMBER 	5 // The ESP8266 Can handle up to 5 connection at the same time
#define MAX_PAYLOAD_SIZE 	300

typedef enum {
	httpGet,
	httpPost,
	httpPut,
	httpDelete
} request_type_t;

typedef struct {
	char* host;
	uint16_t port;
	char* path;
	request_type_t type;
	char* header;
	char* body;
	uint16_t timeout;
} http_request_t;

typedef enum {
	httpSuccess,
	httpBussy,
	httpRequestError,
	http404
} http_error_t;


/*************
 * METHODS
 *************/

/*
 * HTTP Configure
 * Performs a configuration
 */

/*
 * HTTP Request
 * Performs a request specified by the request structure
 *
 * @request Request structure see @typedef http_request_t
 * @buffer 	a buffer where the answer will be stored
 * @size	Size of the buffer
 * @returns	error code
 */
http_error_t HTTP_Request (http_request_t* request, uint8_t* buffer, uint16_t size);


#endif /* SOURCES_LIBS_ESP8266_UART_DRIVER_HTTP_ABSTRACTION_ESP8266_H_ */
