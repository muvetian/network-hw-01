#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

int main(){
  char buffer[4048];
  char filename[128];
  char protocol[16];
  char msg_buffer[6048];
  strcpy(buffer,"GET /book.pdf HTTP/1.0 \r\n\r\n");
  // strcpy(buffer,"PUT /new.html HTTP/1.1 Host: example.com Content-type: text/html Content-length: 16 \r\n\r\n");
  int length = 10;
  header_parse(buffer,4048,filename, 128, protocol,16,&length);
  printf("The filename is: %s and the protocol is: %s\n",filename,protocol);
  fill_reply_403(msg_buffer, filename, protocol);
  printf("the error message is:\n%s",msg_buffer);
  return 0;
}
