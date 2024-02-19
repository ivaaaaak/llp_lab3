#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>


int init_server_socket(int port);
int listen_connections(int server_socket, int max_connections);
int accept_connection(int server_socket);
int receive_data(int client_socket, char *buffer, int buffer_size);
int send_data(int client_socket, char *data);
int close_socket(int client_socket);
int close_connection(int client_socket);

#endif