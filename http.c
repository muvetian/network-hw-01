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
	char* appearance = strstr(buffer, "GET / HTTP/1.");
	if (appearance!= NULL) {
		char protocal_str[protocol_length];
		for(int i = 0; i <  protocol_length;i++){
			protocal_str[i] = appearance[6+i];
		}
		strcpy(protocal_str,protocol);
		char filename_str[filename_length];
		for(int j = 0; j < filename_length;j++){
			filename_str[j] = apperance[14]
		}
	}
	else{
		perror("The request is not legal");
		//return?
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
