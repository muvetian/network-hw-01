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
	int put_flag = 0;
	printf("did it?");
	char* request = strstr(buffer,"GET");
	if(request == NULL){
		request = strstr(buffer,"PUT");
		if( request == NULL || request - buffer != 0){
			fprintf(stderr, "Please provide a legal argument\n");
			return -1;
		}
		else{
			put_flag = 1;
		}
	}
	else if(request - buffer != 0){
		fprintf(stderr, "Please provide a legal argument\n");
		return -1;
	}

	else{
		char* filename_ptr = strstr(buffer, "/") + 1;
		char* protocol_ptr = strstr(buffer,"HTTP/1.");

		*(protocol_ptr-1) = '\0';
		char* protocol_ptr_end = strstr(buffer,"\r\n\r\n");
		*(protocol_ptr_end-1) = '\0';

		if(filename_ptr[0] == '\0'){

			strncpy(filename,"index.html",filename_length);
		}
		else{
			strncpy(filename,filename_ptr,filename_length);
		}

		strncpy(protocol,protocol_ptr,protocol_length);
		if(put_flag == 1){
			char* content_length_ptr = strstr(buffer, "Content-length:");
			char* length = strstr(content_length_ptr," ")+1;
			*content_length = atoi(length);

		}
	}
	if(!buffer || !filename || !protocol || !content_length) {
		fprintf(stderr, "Please provide non-null buffer/filename/protocol/content-length\n");
		return -1;
	}

	if(buffer_length < 128 || filename_length < 128 || protocol_length < 16) {
		fprintf(stderr, "Please provide buffer/filename strings with sizes >= 128 and a protocol string with size >= 16\n");

		return -1;
	}

	buffer[buffer_length] = '\0';

	// TODO: Your code here
	// Starting by finding the "GET /" or "POST /" string using strcasestr()

	return -1;
}

void fill_reply_200(char *buffer, char *filename, char *protocol, int filesize) {
	sprintf(buffer, "%s 200 OK\r\nFilename: %s\r\nContent-Length: %d\r\n\r\n", protocol, filename, filesize);
}

void fill_reply_201(char *buffer, char *filename, char *protocol) {
	sprintf(buffer, "%s 201 Created\r\n\r\n", protocol);
}

void fill_reply_403(char *buffer, char *filename, char *protocol) {
}

void fill_reply_404(char *buffer, char *filename, char *protocol) {
}
