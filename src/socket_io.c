#include "socket_io.h"
#include <stdio.h>

int network_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", GET_SOCKET_ERROR());
        return 0;
    }
#endif
    return 1;
}

socket_t socket_create_listener(void) {
    socket_t server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", GET_SOCKET_ERROR());
    } else {
        int reuse_addr = 1;
        if (setsockopt(server_socket,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (const char *)&reuse_addr,
                       (socklen_t)sizeof(reuse_addr)) == SOCKET_ERROR) {
            printf("Warning: failed to set SO_REUSEADDR. Error Code: %d\n", GET_SOCKET_ERROR());
        }
        printf("Socket created successfully\n");
    }
    return server_socket;
}

int socket_bind(socket_t server_socket, int port) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((unsigned short)port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", GET_SOCKET_ERROR());
        return 0;
    }

    printf("Socket successfully bound to port %d\n", port);
    return 1;
}

int socket_listen(socket_t server_socket) {
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", GET_SOCKET_ERROR());
        return 0;
    }

    printf("Server is listening for connections...\n");
    return 1;
}

socket_t socket_accept(socket_t server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    socket_t client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addrlen);

    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", GET_SOCKET_ERROR());
    } else {
        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    return client_socket;
}

int socket_receive(socket_t client_socket, char *buf, size_t buf_size, size_t *out_len) {
    if (buf == NULL || out_len == NULL || buf_size < 2) {
        return 0;
    }

    int bytes_received = recv(client_socket, buf, (int)(buf_size - 1), 0);
    if (bytes_received == SOCKET_ERROR) {
        printf("recv failed. Error Code: %d\n", GET_SOCKET_ERROR());
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

int socket_send(socket_t socket, const char *buf, size_t len) {
    if (buf == NULL) {
        return 0;
    }

    int bytes_sent = send(socket, buf, (int)len, 0);
    if (bytes_sent == SOCKET_ERROR) {
        printf("send failed. Error Code: %d\n", GET_SOCKET_ERROR());
        return 0;
    }

    printf("Response sent successfully (%d bytes)\n", bytes_sent);
    return 1;
}

void socket_close(socket_t socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

void network_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}
