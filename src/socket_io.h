#ifndef SOCKET_IO_H
#define SOCKET_IO_H

#include <stddef.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#ifdef _WIN32
typedef SOCKET socket_t;
typedef int socklen_t;
#define GET_SOCKET_ERROR() WSAGetLastError()
#else
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define GET_SOCKET_ERROR() errno
#endif

#define PORT 8000
#define BUFFER_SIZE 1024

int network_init(void);
socket_t socket_create_listener(void);
int socket_bind(socket_t server_socket, int port);
int socket_listen(socket_t server_socket);
socket_t socket_accept(socket_t server_socket);
int socket_receive(socket_t client_socket, char *buf, size_t buf_size, size_t *out_len);
int socket_send(socket_t socket, const char *buf, size_t len);
void socket_close(socket_t socket);
void network_cleanup(void);

#endif
