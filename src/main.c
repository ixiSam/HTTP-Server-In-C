#include "server.h"

int main() {
    
    if (!network_init()) return 1;
    socket_t server_socket = create_socket();
    if (server_socket == INVALID_SOCKET) return 1;
    
    if (!bind_socket(server_socket, PORT)) return 1;
    if (!listen_socket(server_socket)) return 1;
    
    while (1) {
        socket_t client_socket = accept_connection(server_socket);
        if (client_socket == INVALID_SOCKET) continue;
        
        receive_data(client_socket);
        send_response(client_socket);
        CLOSE_SOCKET(client_socket);
    }
    
    network_cleanup(server_socket);
    return 0;
}
