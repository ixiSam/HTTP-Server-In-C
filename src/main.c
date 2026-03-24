#include "server.h"

int main(void) {

    if (!network_init()) return 1;
    socket_t server_socket = create_socket();
    if (server_socket == INVALID_SOCKET) return 1;

    if (!bind_socket(server_socket, PORT)) return 1;
    if (!listen_socket(server_socket)) return 1;

    while (1) {
        socket_t client_socket = accept_connection(server_socket);
        if (client_socket == INVALID_SOCKET) continue;

        char raw_buf[BUFFER_SIZE];
        size_t raw_len = 0;
        HttpRequest req;
        ParserResult result = PARSE_ERROR;

        if (receive_data(client_socket, raw_buf, sizeof(raw_buf), &raw_len)) {
            result = http_parse_request(&req, raw_buf, raw_len);
        }

        send_response(client_socket, result);
        CLOSE_SOCKET(client_socket);
    }

    network_cleanup(server_socket);
    return 0;
}
