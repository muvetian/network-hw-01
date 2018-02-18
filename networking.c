/*
 * Copyright (c) 2018, Hammurabi Mendes.
 * License: BSD 2-clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include "networking.h"

#define BUFFER_SIZE 1024

int create_server(int port) {
	int result;
	struct addrinfo result_hints;
	struct addrinfo *result_list;

	memset(&result_hints, 0, sizeof(struct addrinfo));

	result_hints.ai_family = AF_UNSPEC;
	result_hints.ai_socktype = SOCK_STREAM;
	result_hints.ai_flags = AI_PASSIVE;
	result = getaddrinfo(NULL, port, &result_hints, &result_list);

	if(result != 0) {
		perror("Cannot obtain address");

		return -1;
	}

	int listen_socket;

	for(struct addrinfo *result_curr = result_list; result_curr != NULL; result_curr = result_curr->ai_next) {

		// Listening socket creation

		listen_socket = socket(result_curr->ai_family, result_curr->ai_socktype, result_curr->ai_protocol);

		if(listen_socket == -1) {
			continue;
		}

		// Binding to a local address/port

		result = bind(listen_socket, result_curr->ai_addr, result_curr->ai_addrlen);
		if(result == -1) {
			close(listen_socket);
			listen_socket = -1;

			continue;
		}
		// Set listen_socket option to 'reusable', so that if the port it is binded to is not actively listening,
		// it can be used by other sockets.
		int yes=1;
		if (setsockopt(listen_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) == -1) {
		    perror("setsockopt");
		    exit(1);
		}

		print_address_information("Listening in address [%s] port [%s]\n", result_curr->ai_addr, result_curr->ai_addrlen);

		break;
	}

	if(listen_socket == -1) {
		fprintf(stderr, "Not possible to bind to any address/port\n");

		return -1;
	}

	// Listen for connections

	result = listen(listen_socket, 5);
	if(result == -1) {
		perror("Impossible to listen to connections");

		return -1;
	}

	int client_socket;
	struct sockaddr_storage client_socket_address;
	socklen_t client_socket_size;

	client_socket_size = sizeof(struct sockaddr_storage);

	client_socket = accept(listen_socket, (struct sockaddr *) &client_socket_address, &client_socket_size);

	if(client_socket == -1) {
		perror("Cannot accept client");

		return -1;
	}

	// Read from client and echo its messages
	while(1) {
		client_socket = accept(listen_socket, (struct sockaddr *) &client_socket_address, &client_socket_size);
		if(client_socket == -1) {
			perror("Cannot accept client");

			return -1;
		}

		// Read from client and echo its messages
		print_address_information("Connection from client from [%s] port [%s]\n", (struct sockaddr *) &client_socket_address, client_socket_size);

		int pid = fork();
		if(pid != 0) {
			// Server executes this
			close(client_socket);
		}
		else {
			// Client executes this
			handle_client(client_socket);
			close(client_socket);
			// This call is important
			exit(0);
		}
	}
	return 0;
}
int handle_client(int client_socket) {
	// Read from client and echo its messages

	char buffer[BUFFER_SIZE];
	int nread;

	while((nread = read(client_socket, buffer, BUFFER_SIZE - 1)) > 0) {
		buffer[nread] = '\0';
		printf("%s", buffer);
	}

	return 0;
}
void print_address_information(char *template, struct sockaddr *address, int address_size) {
	int result;

	char host[1024];
	char port[16];

	result = getnameinfo(address, address_size, host, 1024, port, 16, NI_NUMERICHOST | NI_NUMERICSERV);

	if(result != 0) {
		perror("Error obtaining information from client");
	}

	printf(template, host, port);
}

int accept_client(int accept_socket) {
	struct sockaddr_storage client_address;
	socklen_t client_length = sizeof(struct sockaddr_storage);

	int client_socket = accept(accept_socket, (struct sockaddr *) &client_address, &client_length);

	return client_socket;
}

void get_peer_information(int socket, char *host_string, int host_length, int *port) {
	int result;

	struct sockaddr_storage address;
	socklen_t length;

	result = getpeername(socket, (struct sockaddr *) &address, &length);

	if(result == -1) {
		perror("getpeername");

		return;
	}

	char port_string[16];

	result = getnameinfo((struct sockaddr *) &address, length, host_string, host_length, port_string, 16, NI_NUMERICHOST | NI_NUMERICSERV);

	*port = atoi(port_string);

	if(result != 0) {
		perror("getnameinfo");

		return;
	}
}
