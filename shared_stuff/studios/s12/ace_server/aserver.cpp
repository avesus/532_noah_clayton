#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"

#define SUCCESS 0
#define FAILURE (-1)

#define PORT_NUM 6005

#define NARGS 1
int main(int argc, char * argv[]){
  if(argc != NARGS){
    fprintf(stderr, "Error invalid arguments passed\n");
    exit(FAILURE);
  }

  fprintf(stderr,
	  "Arg[0]: %s\n",
	  argv[0]);

  ACE_INET_Addr ace_addr(PORT_NUM, ACE_LOCALHOST);
  ACE_SOCK_Stream ace_stream;
  ACE_SOCK_Acceptor ace_accptr;
  if(ace_accptr.open(ace_addr, 1) < 0){
    fprintf(stderr, "Error opening acceptor\n");
    exit(FAILURE);
  }
  char addr_to_str_buf[256] = "";
  ace_addr.addr_to_string(addr_to_str_buf, 256);
  fprintf(stderr, "New Acceptor On ->\n\t%s\n", addr_to_str_buf);

  if(ace_accptr.accept(ace_stream) < 0){
    fprintf(stderr, "Error calling accept\n");
    exit(FAILURE);
  }
  size_t ret = 0;
  char single_read;
  while((ret = ace_stream.recv_n(&single_read, 1)) > 0){
    fprintf(stderr, "%c", single_read);
  }

  return SUCCESS;
}
