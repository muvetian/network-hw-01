#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

int main(){
  char buffer[4048];
  char filename[128];
  char protocol[16];
  strcpy(buffer,"GET /book.pdf HTTP/1.0 \r\n\r\n");
  int length = 10;
  header_parse(buffer,4048,filename, 128, protocol,16,&length);
  printf("the filename is: %s and the protocol is: %s",filename,protocol);
  return 0;
}
