#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
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

#define MIN_ARGS 2

class sigint_handler: public ACE_Event_Handler {
public:
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
	    "TIMEOUT: %d\n\t"
	    "SIGNAL : %d\n",
	    !!(ace_r_mask & TIMER_MASK),
	    !!(ace_r_mask & SIGNAL_MASK));
    handle_signal(SIGINT, NULL, NULL);
    return 0;
  }


class my_ev_handler: public ACE_Event_Handler {
  int n_to_send;
  char ** to_send;
  int_class ** my_int;
  int nptrs;
public:
  void set_interceptor(int_class ** new_int, int n) {
    my_int = new_int;
    nptrs = n;
  }
  my_ev_handler(int send_argc, char ** send_argv) {
    my_int = NULL;
    this->n_to_send = send_argc;
    this->to_send = send_argv;
  }
  
  int handle_timeout (const ACE_Time_Value &current_time,
		      const void *act) {

    ACE_INET_Addr ace_addr(PORT_NUM, ACE_LOCALHOST);
    ACE_SOCK_Stream ace_stream;
    ACE_SOCK_Connector ace_connect;


    if(ace_connect.connect(ace_stream, ace_addr) < 0){
      fprintf(stderr, "Error connecting\n");
      return FAILURE;
    }

    char send_buf[32];
    for(int i=0; i<this->n_to_send; i++){
      sprintf(send_buf, "%s\n", this->to_send[i]);
      int len = strlen(send_buf);
      for(int l =0; my_int && l<len; l++){
	for(int n=0; n<nptrs; n++){
	  (*(my_int[n]))(send_buf[l]);
	}
      }
      if(ace_stream.send_n(send_buf, len) < 0){
	fprintf(stderr, "Error sending (%d)\n", i);
	return FAILURE;
      }
    }
    ace_stream.close();
    return SUCCESS;
  }
};


void errdie(){
  fprintf(stderr, "Timeout occured\n");
  exit(FAILURE);
}

int main(int argc, char * argv[]){
  if(argc < MIN_ARGS){
    fprintf(stderr, "Error no arguments detected\n");
    exit(FAILURE);
  }

    //  pass_through * p = new pass_through;
  int ntest = 50;
  int_class ** ptrs = (int_class **)calloc(ntest, sizeof(a_enc *));
  for(int i=0;i<ntest;i++){
    ptrs[i] = new a_enc(i&0xff);
  }
  my_ev_handler sender(argc, argv);
  sender.set_interceptor(ptrs, ntest);
  sigint_handler h;

  ACE_Reactor::instance()->register_handler(SIGINT,
					    &h);

  ACE_Time_Value t_interval(3, 0);
  ACE_Time_Value t_delay(0, 0);
  ACE_Reactor::instance()->schedule_timer(&sender, 0, t_delay, t_interval);
  ACE_Reactor::instance()->run_reactor_event_loop();
  return SUCCESS;
}
