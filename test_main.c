#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

int notmain(){
  char buffer[4048];
  char filename[128];
  char protocol[16];
  strcpy(buffer,"GET /monsters_inc.jpeg HTTP/1.0 \r\n\r\n");
//  strcpy(buffer,"PUT /putrequest.html HTTP/1.1 Host: example.com Content-Type: text/html Content-Length: 16 \r\n\r\n");
  int length = 10;
  header_parse(buffer,4048,filename, 128, protocol,16,&length);
  printf("the filename is: %s and the protocol is: %s\n",filename,protocol);

  return 0;
}
