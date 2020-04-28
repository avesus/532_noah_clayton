#include <stdio.h>
#include <stdlib.h>
#include <vector>
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


#define ACTIVE 0
#define SUSPENDED 1
#define CANCELLED 2


  std::string
  get_status(int running) {
    if(running == ACTIVE){
      return "Resumed/Running";
    }
    else if(running == SUSPENDED) {
      return "Suspended";
    }
    return "Unknown";
  }


#define SUCCESS 0
#define FAILURE (-1)

#define PORT_NUM 6005

#define NARGS 1


typedef struct service{
  ACE_Service_Object * addr;
  std::string label;
  int status;
} service;

class services : public ACE_Service_Object{
  std::vector<service *> s_list;
  ACE_SOCK_Stream fd;
public:
  services () {
    fd = STDIN_FILENO;
  }
  void add(service * new_s) {
    this->s_list.push_back(new_s);
  }
  void res_sus(ACE_Service_Object * addr, int new_status) {
    for(unsigned int i=0; i<this->s_list.size(); i++){
      if(this->s_list[i]->addr == addr) {
	if(new_status == ACTIVE){
	  this->s_list[i]->addr->resume();
	}
	else if(new_status == SUSPENDED){
	  this->s_list[i]->addr->suspend();
	}
	this->s_list[i]->status = new_status;
      }
    }
  }
  int replace(ACE_Service_Object * addr, std::string label) {
    for(unsigned int i=0; i<this->s_list.size(); i++){
      if(this->s_list[i]->addr == addr) {
	fprintf(stderr,
		"Replacing(%d):\n\tOld: <%p> <%s>\n\tNew: <%p> <%s>\n",
		i,
		addr, this->s_list[i]->label.c_str(),
		addr, label.c_str());
	this->s_list[i]->label = label;
	return 1;
      }
    }
    fprintf(stderr, "Couldnt find replacement for addr <%p>\n", addr);
    return 0;
  }
  service * new_service(ACE_Service_Object * addr, std::string label) {
    service * new_s = (service *)calloc(1, sizeof(service));
    new_s->addr = addr;
    new_s->label = label;
    return new_s;
  }
  void list() {
    for(unsigned int i=0; i<this->s_list.size(); i++){
      fprintf(stderr, "%d: <%p> <%s>\n\tStatus: %s\n",i, this->s_list[i]->addr, this->s_list[i]->label.c_str(), get_status(this->s_list[i]->status).c_str()); 
    }
  }
  int handle_input (ACE_HANDLE h = ACE_INVALID_HANDLE) {
    char buf[64] = "";
    char single_read;
    int index = 0, ret;
    
    while((ret = this->fd.recv_n(&single_read, 1)) > 0){
      buf[index] = single_read;
      if(single_read == '\n'){
	break;
      }

      index++;
    }


    void * addr;
    char label[32] = "";

    int nstat=0;
    if(sscanf(buf, "%p %d\n", &addr, &nstat) == 2){
      this->res_sus((ACE_Service_Object *)addr, nstat);
    }
    
    else if(sscanf(buf, "%p %s\n", &addr, label) == 2){
      this->replace((ACE_Service_Object *)addr, label);
    }

    this->list();
    return 0;
  }

  ACE_HANDLE get_handle() const {
    return this->fd.get_handle();
  }


  

};
  
class my_ev_handler: public ACE_Service_Object {
  ACE_INET_Addr ace_addr;
  ACE_SOCK_Acceptor ace_accptr;
  int_class ** my_int;
  int nptrs;
  int running;
public:
  bool status;
  void set_interceptor(int_class ** new_int, int n) {
    my_int = new_int;
    nptrs = n;
    running = ACTIVE;
  }

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




class sigint_handler: public ACE_Service_Object {
  my_ev_handler * ev;
  int running;
public:
  sigint_handler(my_ev_handler * init) {
    this->ev = init;
    running = ACTIVE;
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
  int ntest = 50;
  int_class ** ptrs = (int_class **)calloc(ntest, sizeof(a_enc *));
  for(int i=0;i<ntest;i++){
    ptrs[i] = new a_enc((i&0xff));
  }

  ACE_SOCK_Acceptor accptr;
  my_ev_handler * sender = new my_ev_handler(accptr);
  sender->set_interceptor(ptrs, ntest);
  sigint_handler h(sender);

  services s;
  s.add(s.new_service(sender, "my_ev_handler"));
  s.add(s.new_service(&h, "sigint_handler"));
  s.list();
  
  ACE_Reactor::instance()->register_handler(sender,
					    ACE_Service_Object::ACCEPT_MASK);
  ACE_Reactor::instance()->register_handler(ACE_STDIN,
					    &s,
					    ACE_Event_Handler::READ_MASK);
  ACE_Reactor::instance()->register_handler(SIGINT,
					    &h);

  ACE_Reactor::instance()->run_reactor_event_loop();
  //  while(sender.handle_input() == SUCCESS);


  return FAILURE;
}
