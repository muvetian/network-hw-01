/*
 * Copyright (c) 2018, Hammurabi Mendes.
 * License: BSD 2-clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>

#include "clients_common.h"
#include "networking.h"

void setup_signal_handler(int signal, void (*handler)(int));
void handle_termination(int signal);
void handle_children(int signal);

static int done = 0;
static atomic_ulong operations_completed;

int main(int argc, char **argv) {
//	if(argc < 2) {
//		fprintf(stderr, "Usage: server <port>\n");
//
//		return EXIT_FAILURE;
//	}

	char *port = "3000"; //argv[1];

	int accept_socket = create_server(atoi(port));

	if(accept_socket == -1) {
		fprintf(stderr, "Error creating server\n");

		return EXIT_FAILURE;
	}

	// Treat signals
	setup_signal_handler(SIGPIPE, SIG_IGN);
	setup_signal_handler(SIGTERM, handle_termination);
	setup_signal_handler(SIGCHLD, handle_children);

	while(!done) {
		char host[1024];
		int port;

		int client_socket = accept_client(accept_socket);

		if(client_socket == -1) {
			if(errno != EINTR) {
				perror("accept");

				exit(EXIT_FAILURE);
			}

			continue;
		}

		get_peer_information(client_socket, host, 1024, &port);
		printf("New connection from %s, port %d\n", host, port);

		pid_t pid = fork();

		if(pid == 0) {
			struct client *client = make_client(client_socket);

			if(read_request(client)) {
				write_reply(client);
			}

			exit(client->status);
		}

		close(client_socket);
	}

	printf("Finishing program cleanly... %ld operations served\n", operations_completed);

	return EXIT_SUCCESS;
}

void setup_signal_handler(int signal, void (*handler)(int)) {
	struct sigaction request;

	memset(&request, 0, sizeof(struct sigaction));

	request.sa_handler = handler;

	if(sigaction(signal, &request, NULL) == -1) {
		perror("sigaction");

		exit(EXIT_FAILURE);
	}
}

void handle_termination(int signal) {
	done = 1;
}

void handle_children(int signal) {
	pid_t returnedChild;
	int returnStatus;

	while((returnedChild = wait(&returnStatus)) != -1) {

		if(WIFEXITED(returnStatus) && WEXITSTATUS(returnStatus) == STATUS_OK) {
			operations_completed++;
		}
	}
}
