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
#include "../ace_interceptor/aint.h"
#include "../ace_interceptor/apass.h"
#include "../ace_interceptor/aenc.h"

#define SUCCESS 0
#define FAILURE (-1)

#define PORT_NUM 6005

#define NARGS 1



class my_ev_handler: public ACE_Event_Handler {
  ACE_INET_Addr ace_addr;
  ACE_SOCK_Acceptor ace_accptr;
  int_class ** my_int;
  int nptrs;
public:
  void set_interceptor(int_class ** new_int, int n) {
    my_int = new_int;
    nptrs = n;
  }
  bool status;
  my_ev_handler(ACE_SOCK_Acceptor &accptr) : ace_accptr(accptr), ace_addr(PORT_NUM, ACE_LOCALHOST) {
    my_int = NULL;
    status = true;
    if(ace_accptr.open(ace_addr, 1) < 0){
      fprintf(stderr, "Error opening acceptor\n");
    }
  }
  ~my_ev_handler() {
    status = false;
  }


  int handle_close (ACE_HANDLE ace_handle, ACE_Reactor_Mask ace_r_mask){

    fprintf(stderr,
	    "Closing...\n\t"
	    "READ  : %d\n\t"
	    "Bool  : %d\n",
	    !!(ace_r_mask & READ_MASK),
	    this->status);
    if(this->status == true){
      delete this;
    }
    return 0;
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
      if(my_int){
	for(int n=0; n < nptrs; n++){
	  (*(my_int[n]))(single_read);
	}
      }
      fprintf(stderr, "%c", single_read);
    }
    ace_stream.close();
    return SUCCESS;
  }



  ACE_HANDLE get_handle() const {
    return this->ace_accptr.get_handle();
  }
};




class sigint_handler: public ACE_Event_Handler {
  my_ev_handler * ev;
public:
  sigint_handler(my_ev_handler * init) {
    this->ev = init;
  }
  virtual int handle_signal (int, siginfo_t* = 0, siginfo_t* = 0);
  virtual int handle_close (ACE_HANDLE, ACE_Reactor_Mask);
};

  int
  sigint_handler::handle_signal (int, siginfo_t *, siginfo_t *){
    
      ACE_Reactor::instance()->end_reactor_event_loop();
      ACE_Reactor::instance()->close();


    return 0;
  }
  int
  sigint_handler::handle_close (ACE_HANDLE ace_handle, ACE_Reactor_Mask ace_r_mask){

    fprintf(stderr,
	    "Closing...\n\t"
	    "ACCEPT: %d\n\t"
	    "SIGNAL: %d\n\t"
	    "Bool  : %d\n",
	    !!(ace_r_mask & ACCEPT_MASK),
	    !!(ace_r_mask & SIGNAL_MASK),
	    this->ev->status);
    handle_signal(SIGINT, NULL, NULL);
    return 0;
  }

int main(int argc, char * argv[]){
  if(argc != NARGS){
    fprintf(stderr, "Error invalid arguments passed\n");
    exit(FAILURE);
  }

  fprintf(stderr,
	  "Arg[0]: %s\n",
	  argv[0]);

  //  pass_through * p = new pass_through;
  int ntest = 49;
  int_class ** ptrs = (int_class **)calloc(ntest, sizeof(a_enc *));
  for(int i=0;i<ntest;i++){
    ptrs[i] = new a_enc((i&0xff));
  }

  ACE_SOCK_Acceptor accptr;
  my_ev_handler * sender = new my_ev_handler(accptr);
  sender->set_interceptor(ptrs, ntest);
  sigint_handler h(sender);

  ACE_Reactor::instance()->register_handler(sender,
					    ACE_Event_Handler::ACCEPT_MASK);
  ACE_Reactor::instance()->register_handler(SIGINT,
					    &h);

  ACE_Reactor::instance()->run_reactor_event_loop();
  //  while(sender.handle_input() == SUCCESS);



  return FAILURE;
}
