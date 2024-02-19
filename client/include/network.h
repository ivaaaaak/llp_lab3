#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define RECEIVE_BUFFER_SIZE 500000

int init_client_socket();
int connect_to_server(int client_socket, char *host, int port);
int send_data(int client_socket, char *data);
int receive_data(int client_socket, char *buffer, int buffer_size);
int close_socket(int client_socket);
int close_connection(int client_socket);

#endif