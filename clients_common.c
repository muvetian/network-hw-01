/*
 * Copyright (c) 2018, Hammurabi Mendes.
 * License: BSD 2-clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "clients_common.h"
#include "networking.h"
#include "http.h"

int flush_buffer(struct client *client);
int obtain_file_size(char *filename);

struct client *make_client(int socket) {
	struct client *new_client = (struct client *) malloc(sizeof(struct client));

	if(new_client != NULL) {
		new_client->socket = socket;
		new_client->state = E_RECV_REQUEST;

		new_client->file = NULL;

		new_client->nread = 0;
		new_client->nwritten = 0;
		new_client->ntowrite = 0;

		new_client->status = STATUS_OK;

		new_client->prev = NULL;
		new_client->next = NULL;
	}

	return new_client;
}

int read_request(struct client *client) {
	int result;

	int nread;
	//using a while loop so that read function will be executed several times
	while((nread = read(client->socket, client->buffer + client->nread, BUFFER_SIZE - 1 - client->nread)) > 0) {
		client->nread = client->nread + nread;

		if(header_complete(client->buffer, client->nread)) {
			// If you want to print what's in the request
			printf("Request:\n%s\n", client->buffer);

			if((client->method = header_parse(client->buffer, client->nread, client->filename, 128, client->protocol, 16, &client->content_length)) == -1) {
				fprintf(stderr, "Invalid header from client socket no. %d - closing connection\n", client->socket);

				client->status = STATUS_BAD;
				finish_client(client);

				return 0;
			}

			client->state = E_SEND_REPLY;
			break;


		}
	}

	return 1;
}

void handle_get(struct client *client) {
	char temporary_buffer[BUFFER_SIZE];

	char *filename = client->filename;
	char *protocol = client->protocol;



	// If the file does not exist... make the 404 header
	if(access(filename, F_OK) != 0) {
		fill_reply_404(temporary_buffer, filename, protocol);
		client->status = STATUS_404;
	}
	// If you cannot open the file in client->file... make the 403 header
	else if((client->file = fopen(filename, "r")) == NULL) {
		fill_reply_403(temporary_buffer, filename, protocol);
		client->status = STATUS_403;
	}
	// Otherwise, client->file is non-null and you have not only the header, but the file data to send
	else {
		fill_reply_200(temporary_buffer, filename, protocol, obtain_file_size(filename));

	}

	// If you want to print what's in the response
	printf("Response:\n%s\n", temporary_buffer);
	strcpy(client->buffer, temporary_buffer);
	client->ntowrite = strlen(client->buffer);
	client->nwritten = 0;
	flush_buffer(client);


	// TODO: Flush the HTTP response header (copied above) to the client, and then, in a loop:
	//         (i) read a chunk from the file into temporary_buffer;
	//         (ii) flushes that chunk to the client
	int file_size = obtain_file_size(filename);

	for(int i = 0; i < file_size/BUFFER_SIZE; i++){
		fread(client->buffer,BUFFER_SIZE,1,client->file);
		client->ntowrite = BUFFER_SIZE;
		client->nwritten = 0;
		flush_buffer(client);
	}
	if(fread(client->buffer,file_size%BUFFER_SIZE,1,client->file)){
		client->ntowrite = file_size%BUFFER_SIZE;
		client->nwritten = 0;
		flush_buffer(client);
	} else {
		client->file = NULL;
		client->status = STATUS_BAD;
	}

	fclose(client->file);
	client->status = STATUS_OK;
	finish_client(client);
}

void handle_put(struct client *client) {
	char temporary_buffer[BUFFER_SIZE];

	char *filename = client->filename;
	char *protocol = client->protocol;

	// If you cannot open the file in client->file... make the 403 header
	if((client->file = fopen(filename, "w")) == NULL) {
		fill_reply_403(temporary_buffer, filename, protocol);
		client->status = STATUS_403;
		// flush buffer here
		finish_client(client);
		return;
	}

	// If you want to print what's in the response
	printf("Response:\n%s\n", temporary_buffer);


	// TODO: In a loop:
	//         (i) read a chunk from the client into temporary_buffer, up to client->content_length;
	//         (ii) writes that chunk into the file opened above using fwrite()
	//       Then, copy the 201 Created header into the buffer, and flush it back to the client
	int nread = 0;
	while((nread = read(client->socket, temporary_buffer, BUFFER_SIZE - 1)) > 0) {
			client->nread = client->nread + nread;
			fwrite(temporary_buffer,BUFFER_SIZE,1,client->file);
	}

	if(fread(temporary_buffer,client->content_length%BUFFER_SIZE,1,client->file)){
		fwrite(temporary_buffer,client->content_length%BUFFER_SIZE,1,client->file);
	} else {
		client->file = NULL;
		fclose(client->file);
		client->status = STATUS_BAD;
	}

	fclose(client->file);
	client->status = STATUS_OK;
	fill_reply_201(temporary_buffer,filename,protocol);
	strcpy(client->buffer, temporary_buffer);
	// flush buffer here too
	finish_client(client);
}

void write_reply(struct client *client) {
	if(client->method == METHOD_GET) {
		handle_get(client);
	}
	else {
		handle_put(client);
	}
}

int flush_buffer(struct client *client) {
	int result;

	while(client->ntowrite) {
		result = write(client->socket, client->buffer + client->nwritten, client->ntowrite);

		if(result == -1) {
			fprintf(stderr, "Cannot write to client socket no. %d - closing connection\n", client->socket);

			return 0;
		}

		client->nwritten += result;
		client->ntowrite -= result;
	}

	return 1;
}

int obtain_file_size(char *filename) {
	struct stat file_information;

	int result = stat(filename, &file_information);

	if(result == -1) {
		return -1;
	}

	return file_information.st_size;
}

void finish_client(struct client *client) {
	char host[1024];
	int port;

	get_peer_information(client->socket, host, 1024, &port);
	printf("Closing connection from %s, port %d\n", host, port);

	close(client->socket);

	client->socket = -1;
}
