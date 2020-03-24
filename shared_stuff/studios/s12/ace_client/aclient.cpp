#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"

#define SUCCESS 0
#define FAILURE (-1)

#define PORT_NUM 6005

#define MIN_ARGS 2
int main(int argc, char * argv[]){
  if(argc < MIN_ARGS){
    fprintf(stderr, "Error no arguments detected\n");
    exit(FAILURE);
  }


  ACE_INET_Addr ace_addr(PORT_NUM, ACE_LOCALHOST);
  ACE_SOCK_Stream ace_stream;
  ACE_SOCK_Connector ace_connect;


  if(ace_connect.connect(ace_stream, ace_addr) < 0){
    fprintf(stderr, "Error connecting\n");
    exit(FAILURE);
  }

  char send_buf[256] = "";
    for(int i=0; i<argc; i++){
      sprintf(send_buf, "%s\n", argv[i]);
      if(ace_stream.send_n(send_buf, strlen(send_buf)) < 0){
	fprintf(stderr, "Error sending (%d)\n", i);
      }
    }
    ace_stream.close();


  return SUCCESS;
}
