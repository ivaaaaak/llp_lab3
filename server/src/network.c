#include "../include/network.h"

int init_server_socket(int port) {
    printf("Initializing server socket on port %d\n", port);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error opening socket");
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        return -1;
    }

    return server_socket;
}

int listen_connections(int server_socket, int max_connections) {
    printf("Listening connections\n");
    if (listen(server_socket, max_connections) < 0) {
        perror("Error listening connections");
        return -1;
    }
    return server_socket;
}

int accept_connection(int server_socket) {
    printf("Accepting connection\n");
    struct sockaddr_in client_address;
    int client_address_size = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr *) &client_address, (socklen_t *) &client_address_size);
    if (client_socket < 0) {
        perror("Error accepting connection");
        return -1;
    }
    return client_socket;
}

int receive_data(int client_socket, char *buffer, int buffer_size) {
    printf("Receiving data\n");
    int received_bytes = recv(client_socket, buffer, buffer_size, 0);
    if (received_bytes < 0) {
        perror("Error receiving data");
        return -1;
    }
    return received_bytes;
}

int send_data(int client_socket, char *data) {
    printf("Sending data\n");
    int data_size = strlen(data);
    int sent_bytes = send(client_socket, data, data_size, 0);
    if (sent_bytes < 0) {
        perror("Error sending data");
        return -1;
    }
    return sent_bytes;
}

int close_socket(int client_socket) {
    printf("Closing socket\n");
    int closed_socket = close(client_socket);
    if (closed_socket < 0) {
        perror("Error closing socket");
        return -1;
    }
    return closed_socket;
}

int close_connection(int client_socket) {
    printf("Closing connection\n");
    return close_socket(client_socket);
}