#include "../include/network.h"


int init_client_socket() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error opening socket");
        return -1;
    }
    return client_socket;

}

int connect_to_server(int client_socket, char *host, int port) {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(host);

    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Error connecting to server");
        return -1;
    }
    return client_socket;
}

int send_data(int client_socket, char *data) {
    int data_size = strlen(data);
    int sent_bytes = send(client_socket, data, data_size, 0);
    if (sent_bytes < 0) {
        perror("Error sending data");
        return -1;
    }
    return sent_bytes;
}

int receive_data(int client_socket, char *buffer, int buffer_size) {
    int received_bytes = recv(client_socket, buffer, buffer_size, 0);
    if (received_bytes < 0) {
        perror("Error receiving data");
        return -1;
    }
    return received_bytes;
}

int close_socket(int client_socket) {
    int closed_socket = close(client_socket);
    if (closed_socket < 0) {
        perror("Error closing socket");
        return -1;
    }
    return closed_socket;
}

int close_connection(int client_socket) {
    int closed_connection = shutdown(client_socket, SHUT_RDWR);
    if (closed_connection < 0) {
        perror("Error closing connection");
        return -1;
    }
    return closed_connection;
}
