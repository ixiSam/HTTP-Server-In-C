#include "server.h"
#include <stdio.h>
#include <string.h>

static char buffer[BUFFER_SIZE];
static char resp[] = "HTTP/1.0 200 OK\r\n"
                     "Server: webserver-c\r\n"
                     "Content-type: text/html\r\n\r\n"
                     "<html>The server sent this HTML!</html>\r\n";

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

int receive_data(socket_t client_socket) {
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == SOCKET_ERROR) {
        printf("recv failed. Error Code: %d\n", GET_SOCKET_ERROR());
        CLOSE_SOCKET(client_socket);
        return 0;
    }
    buffer[bytes_received] = '\0';
    printf("Received: %s\n", buffer);
    return 1;
}

int send_response(socket_t client_socket) {
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
