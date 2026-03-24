#include "http_response.h"
#include <string.h>

static const char resp_200[] = "HTTP/1.0 200 OK\r\n"
                               "Server: webserver-c\r\n"
                               "Content-type: text/html\r\n\r\n"
                               "<html>The server sent this HTML!</html>\r\n";

static const char resp_400[] = "HTTP/1.0 400 Bad Request\r\n"
                               "Server: webserver-c\r\n"
                               "Content-type: text/plain\r\n\r\n"
                               "Bad Request\r\n";

static const char resp_413[] = "HTTP/1.0 413 Payload Too Large\r\n"
                               "Server: webserver-c\r\n"
                               "Content-type: text/plain\r\n\r\n"
                               "Payload Too Large\r\n";

static const char resp_505[] = "HTTP/1.0 505 HTTP Version Not Supported\r\n"
                               "Server: webserver-c\r\n"
                               "Content-type: text/plain\r\n\r\n"
                               "HTTP Version Not Supported\r\n";

static const char resp_500[] = "HTTP/1.0 500 Internal Server Error\r\n"
                               "Server: webserver-c\r\n"
                               "Content-type: text/plain\r\n\r\n"
                               "Internal Server Error\r\n";

static const char *response_for_parse_result(ParserResult result) {
    switch (result) {
        case PARSE_OK:
            return resp_200;
        case PARSE_BAD_REQUEST:
            return resp_400;
        case PARSE_TOO_LARGE:
            return resp_413;
        case PARSE_UNSUPPORTED_VERSION:
            return resp_505;
        default:
            return resp_500;
    }
}

int http_send_response(socket_t client_socket, ParserResult result) {
    const char *resp = response_for_parse_result(result);
    return socket_send(client_socket, resp, strlen(resp));
}
