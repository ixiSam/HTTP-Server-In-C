#include "socket_io.h"
#include "parser.h"
#include "http_response.h"

int main(void) {

    if (!network_init()) return 1;
    socket_t server_socket = socket_create_listener();
    if (server_socket == INVALID_SOCKET) return 1;

    if (!socket_bind(server_socket, PORT)) {
        socket_close(server_socket);
        network_cleanup();
        return 1;
    }

    if (!socket_listen(server_socket)) {
        socket_close(server_socket);
        network_cleanup();
        return 1;
    }

    while (1) {
        socket_t client_socket = socket_accept(server_socket);
        if (client_socket == INVALID_SOCKET) continue;

        char raw_buf[BUFFER_SIZE];
        size_t raw_len = 0;
        HttpRequest req;
        ParserResult result = PARSE_ERROR;

        if (socket_receive(client_socket, raw_buf, sizeof(raw_buf), &raw_len)) {
            result = http_parse_request(&req, raw_buf, raw_len);
        }

        http_send_response(client_socket, result);
        socket_close(client_socket);
    }

    socket_close(server_socket);
    network_cleanup();
    return 0;
}
