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

int create_server(int port) {
	int result;
	struct addrinfo result_hints;
	struct addrinfo *result_list;

	char str[128];
	sprintf(str, "%d", port);

	memset(&result_hints, 0, sizeof(struct addrinfo));

	result_hints.ai_family = AF_UNSPEC;
	result_hints.ai_socktype = SOCK_STREAM;
	result_hints.ai_flags = AI_PASSIVE;
	result = getaddrinfo(NULL, str, &result_hints, &result_list);

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

		int yes=1;
		// lose the pesky "Address already in use" error message
		if (setsockopt(listen_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
			perror("setsockopt");
			exit(1);
		}

		// Binding to a local address/port

		result = bind(listen_socket, result_curr->ai_addr, result_curr->ai_addrlen);

		if(result == -1) {
			close(listen_socket);
			listen_socket = -1;

			continue;
		}



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

	// Read from client and echo its messages

	return listen_socket;
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
