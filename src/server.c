#include "server.h"
#include <stdio.h>
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

int network_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", GET_SOCKET_ERROR());
        return 0;
    }
    return 1; 
#endif
    return 1;
}

socket_t create_socket(void) {
    socket_t server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", GET_SOCKET_ERROR());
    } else {
        printf("Socket created successfully\n");
    }
    return server_socket;
}

int bind_socket(socket_t server_socket, int port) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", GET_SOCKET_ERROR());
        CLOSE_SOCKET(server_socket);
        return 0;
    }
    printf("Socket successfully bound to port %d\n", port);
    return 1;
}

int listen_socket(socket_t server_socket) {
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", GET_SOCKET_ERROR());
        CLOSE_SOCKET(server_socket);
        return 0;
    }
    printf("Server is listening for connections...\n");
    return 1;
}

socket_t accept_connection(socket_t server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    socket_t client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addrlen);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", GET_SOCKET_ERROR());
    } else {
        printf("Connection accepted from %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
    }
    return client_socket;
}

int receive_data(socket_t client_socket, char *buf, size_t buf_size, size_t *out_len) {
    if (buf == NULL || out_len == NULL || buf_size < 2) {
        return 0;
    }

    int bytes_received = recv(client_socket, buf, (int)(buf_size - 1), 0);
    if (bytes_received == SOCKET_ERROR) {
        printf("recv failed. Error Code: %d\n", GET_SOCKET_ERROR());
        CLOSE_SOCKET(client_socket);
        return 0;
    }

    if (bytes_received <= 0) {
        *out_len = 0;
        return 0;
    }

    buf[bytes_received] = '\0';
    *out_len = (size_t)bytes_received;
    printf("Received: %s\n", buf);
    return 1;
}

int send_response(socket_t client_socket, ParserResult result) {
    const char *resp = resp_500;

    switch (result) {
        case PARSE_OK:
            resp = resp_200;
            break;
        case PARSE_BAD_REQUEST:
            resp = resp_400;
            break;
        case PARSE_TOO_LARGE:
            resp = resp_413;
            break;
        case PARSE_UNSUPPORTED_VERSION:
            resp = resp_505;
            break;
        default:
            resp = resp_500;
            break;
    }

    int bytes_sent = send(client_socket, resp, strlen(resp), 0);
    if (bytes_sent == SOCKET_ERROR) {
        printf("send failed. Error Code: %d\n", GET_SOCKET_ERROR());
        CLOSE_SOCKET(client_socket);
        return 0;
    }
    printf("Response sent successfully (%d bytes)\n", bytes_sent);
    return 1;
}

void network_cleanup(socket_t server_socket) {
    CLOSE_SOCKET(server_socket);
#ifdef _WIN32
    WSACleanup();
#endif
}
