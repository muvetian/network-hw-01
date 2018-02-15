/*
 * Copyright (c) 2018, Hammurabi Mendes.
 * License: BSD 2-clause
 */
#ifndef HTTP_H
#define HTTP_H

#define METHOD_GET 0
#define METHOD_PUT 1

/**
 * @param buffer Pointer to the buffer where the HTTP response will be written (assumed to be BUFFER_SIZE of length).
 * @param filename Pointer to the character buffer containing the filename requested by the client.
 * @param protocol Pointer to the character buffer containing the protocol used by the client.
 * @param filesize Length of the file requested by the client.
 */
void fill_reply_200(char *buffer, char *filename, char *protocol, int filesize);

/**
 * @param buffer Pointer to the buffer where the HTTP response will be written (assumed to be BUFFER_SIZE of length).
 * @param filename Pointer to the character buffer containing the filename requested by the client.
 * @param protocol Pointer to the character buffer containing the protocol used by the client.
 */
void fill_reply_201(char *buffer, char *filename, char *protocol);

/**
 * @param buffer Pointer to the buffer where the HTTP response will be written (assumed to be BUFFER_SIZE of length).
 * @param filename Pointer to the character buffer containing the filename requested by the client (but not accessible).
 * @param protocol Pointer to the character buffer containing the protocol used by the client.
 */
void fill_reply_403(char *buffer, char *filename, char *protocol);

/**
 * @param buffer Pointer to the buffer where the HTTP response will be written (assumed to be BUFFER_SIZE of length).
 * @param filename Pointer to the character buffer containing the filename requested by the client (but not existent).
 * @param protocol Pointer to the character buffer containing the protocol used by the client.
 */
void fill_reply_404(char *buffer, char *filename, char *protocol);

/**
 * Returns true if the buffer contains a complete HTTP request header.
 *
 * NOTE: This function will add a null character to the end of the buffer, so make sure that it contains
 *       at least one extra byte available for that.
 *
 * @param buffer Pointer to the character buffer where data is being received.
 * @param buffer_length Length of the character buffer where data is being received.
 *
 * @return 1 if the header is complete; 0 otherwise.
 */
int header_complete(char *buffer, int buffer_length);

/**
 * Parses an HTTP header and fills up the passed fields (assuming it is complete - otherwise the behaviour is undefined).
 *
 * @param buffer Pointer to the buffer containing a complete (and null-terminated) HTTP header.
 * @param buffer_length Length of the buffer containing a complete (and null-terminated) HTTP header.
 * @param filename Pointer to the character buffer where the filename will be stored.
 * @param filename_length Length of the character buffer where the filename will be stored.
 * @param protocol Pointer to the character buffer where the protocol will be stored.
 * @param protocol_length Length of the character buffer where the protocol will be stored.
 * @param content_length Pointer to an integer that will be filled up with the content length of the HTTP PUT request.
 *
 * @return 0 if the filename/protocol have been parsed successfully; -1 otherwise.
 */
int header_parse(char *buffer, int buffer_length, char *filename, int filename_length, char *protocol, int protocol_length, int *content_length);
#endif /* HTTP_H */
