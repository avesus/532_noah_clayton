#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Event_Handler.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/OS.h"
#include "ace/Reactor.h"
#include "ace/Timer_Heap_T.h"
#include "ace/Timer_Heap.h"
#include "ace/Event_Handler_Handle_Timeout_Upcall.h"


#define SUCCESS 0
#define FAILURE (-1)

#define PORT_NUM 6005

#define NARGS 1



class my_ev_handler: public ACE_Event_Handler {
  ACE_INET_Addr ace_addr;
  ACE_SOCK_Acceptor ace_accptr;
public:
  my_ev_handler(ACE_SOCK_Acceptor &accptr) : ace_accptr(accptr), ace_addr(PORT_NUM, ACE_LOCALHOST) {
    if(ace_accptr.open(ace_addr, 1) < 0){
      fprintf(stderr, "Error opening acceptor\n");
    }
  }

  int handle_input (ACE_HANDLE h = ACE_INVALID_HANDLE) {

    ACE_SOCK_Stream ace_stream;
    
    char addr_to_str_buf[256] = "";
    ace_addr.addr_to_string(addr_to_str_buf, 256);
    fprintf(stderr, "New Acceptor On ->\n\t%s\n", addr_to_str_buf);

    if(ace_accptr.accept(ace_stream) < 0){
      fprintf(stderr, "Error calling accept\n");
      return FAILURE;
    }
    size_t ret = 0;
    char single_read;
    while((ret = ace_stream.recv_n(&single_read, 1)) > 0){
      fprintf(stderr, "%c", single_read);
    }
    ace_stream.close();
    return SUCCESS;
  }



  ACE_HANDLE get_handle() const {
    return this->ace_accptr.get_handle();
  }
 
};


int main(int argc, char * argv[]){
  if(argc != NARGS){
    fprintf(stderr, "Error invalid arguments passed\n");
    exit(FAILURE);
  }

  fprintf(stderr,
	  "Arg[0]: %s\n",
	  argv[0]);

  ACE_SOCK_Acceptor accptr;
  my_ev_handler sender(accptr);
  ACE_Reactor::instance()->register_handler(&sender,
					    ACE_Event_Handler::ACCEPT_MASK);
  ACE_Reactor::instance()->run_reactor_event_loop();
  //  while(sender.handle_input() == SUCCESS);

    

  return FAILURE;
}
