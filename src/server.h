#ifndef SERVER_H
#define SERVER_H

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
#define CLOSE_SOCKET closesocket
#define GET_SOCKET_ERROR() WSAGetLastError()
#else
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define CLOSE_SOCKET close
#define GET_SOCKET_ERROR() errno
#endif

#define PORT 8000
#define BUFFER_SIZE 1024

int network_init(void);
socket_t create_socket(void);
int bind_socket(socket_t server_socket, int port);
int listen_socket(socket_t server_socket);
socket_t accept_connection(socket_t server_socket);
int receive_data(socket_t client_socket);
int send_response(socket_t client_socket);
void network_cleanup(socket_t server_socket);

#endif // SERVER_H
