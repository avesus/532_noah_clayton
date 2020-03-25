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

#define SUCCESS 0
#define FAILURE (-1)

#define PORT_NUM 6005

#define MIN_ARGS 2


class my_ev_handler: public ACE_Event_Handler {
  int n_to_send;
  char ** to_send;
public:
  my_ev_handler(int send_argc, char ** send_argv) {
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
      if(ace_stream.send_n(send_buf, strlen(send_buf)) < 0){
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


  my_ev_handler sender(argc, argv);


  ACE_Time_Value t_interval(3, 0);
  ACE_Time_Value t_delay(0, 0);
  ACE_Reactor::instance()->schedule_timer(&sender, 0, t_delay, t_interval);
  ACE_Reactor::instance()->run_reactor_event_loop();
  return SUCCESS;
}
