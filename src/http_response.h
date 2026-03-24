#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "socket_io.h"
#include "parser.h"

int http_send_response(socket_t client_socket, ParserResult result);

#endif
