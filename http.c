/*
 * Copyright (c) 2018, Hammurabi Mendes.
 * License: BSD 2-clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

int header_complete(char *buffer, int buffer_length) {
	buffer[buffer_length] = '\0';

	if(strstr(buffer, "\r\n\r\n")) {
		return 1;
	}

	return 0;
}

int header_parse(char *buffer, int buffer_length, char *filename, int filename_length, char *protocol, int protocol_length, int *content_length) {

	// TODO: Your code here
	// Starting by finding the "GET /" or "POST /" string using strcasestr()
	if(strstr(buffer,"PUT")){
		char* content_length_ptr = strstr(buffer, "Content-Length:");
		char* length = strstr(content_length_ptr," ")+1;
		*content_length = atoi(length);
	} else if (strstr(buffer,"GET") == NULL) {
		fprintf(stderr, "Please provide a legal argument\n");
		return -1;
	}

	char* filename_ptr = strstr(buffer, "/")+1;
	char* protocol_ptr = strstr(buffer,"HTTP/1.");
	*(protocol_ptr+8) = '\0';
	*(protocol_ptr-1) = '\0';

	if(filename_ptr[0] == '\0'){
		strncpy(filename,"index.html",filename_length);
	} else {
		strncpy(filename,filename_ptr,filename_length);
	}

	strncpy(protocol,protocol_ptr,protocol_length);

	if(!buffer || !filename || !protocol || !content_length) {
		fprintf(stderr, "Please provide non-null buffer/filename/protocol/content-length\n");
		return -1;
	}

	if(buffer_length < 128 || filename_length < 128 || protocol_length < 16) {
		fprintf(stderr, "Please provide buffer/filename strings with sizes >= 128 and a protocol string with size >= 16\n");

		return -1;
	}

	buffer[buffer_length] = '\0';

	return METHOD_GET;
}

void fill_reply_200(char *buffer, char *filename, char *protocol, int filesize) {
	sprintf(buffer, "%s 200 OK\r\nFilename: %s\r\nContent-Length: %d\r\n\r\n", protocol, filename, filesize);
}

void fill_reply_201(char *buffer, char *filename, char *protocol) {
	sprintf(buffer, "%s 201 Created\r\n\r\n", protocol);
}

void fill_reply_403(char *buffer, char *filename, char *protocol) {
	char msg_buffer[4048];
	msg_buffer[0] = '\0';
	strcat(msg_buffer,protocol);
	strcat(msg_buffer,"403 Forbidden\nFilename: ");
	strcat(msg_buffer,filename);
	strcat(msg_buffer,"\n\n<HTML><p>Sorry, permission is not granted</p></HTML>");
	strcpy(buffer,msg_buffer);
}

void fill_reply_404(char *buffer, char *filename, char *protocol) {
	char msg_buffer[4048];
	msg_buffer[0] = '\0';
	strcat(msg_buffer,protocol);
	strcat(msg_buffer,"404 Not Found\nFilename: ");
	strcat(msg_buffer,filename);
	strcat(msg_buffer,"\n\n<HTML><p>Whoops, the file you are looking for is not here</p></HTML>");

	strcpy(buffer,msg_buffer);

}
