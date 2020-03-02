#include "s10.h"
#include "watcher.h"

#define SUCCESS 0
#define FAILURE -1


int verbose = 0;

#define Version "0.1"

static ArgOption args[] = {
  // Kind, 	  Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
static ArgDefs argp = { args, "s10", Version, NULL };

using namespace std;

unsigned long rec_fact(class event_logger* el, unsigned long f, unsigned long tid){
  
  el->new_tid(tid);
  struct event_info ei;
  const char* func = "factorial";
  ei.file = __FILE__;
  ei.line = __LINE__;
  ei.ev = CALLED;
  ei.func = (void*)func;

  if(f == 1){
    ei.ev = RETURNING;
    el->ev_add(tid, ei);
    return 1;
  }
  else{
    ei.ev = RETURNING;
    el->ev_add(tid, ei);
    return f*rec_fact(el, f-1, tid);
  }

}

int main(int argc, char* argv[]){
  class event_logger el(cout);
  const char* func = "main";
  el._register((void *)func, std::string(func));
  el.new_tid(0);
  struct event_info ei;
  ei.file = __FILE__;
  ei.line = __LINE__;
  ei.ev = CALLED;
  ei.func = (void*)func;
  el.ev_add(0, ei);
  el << ei;
  cout << ei;
  progname = argv[0];
  ArgParser* ap = createArgumentParser(&argp);
  int result = parseArguments(ap, argc, argv);
  if(result){
    die("Error parsing arguments");
  }
  freeCommandLine();
  freeArgumentParser(ap);
  rec_fact(&el, 20, 1);
  ei.ev = RETURNING;
  el << ei;
  cout << ei;
  el.print();
  return SUCCESS;
}
