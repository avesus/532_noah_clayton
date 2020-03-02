#include "s9.h"


#define SUCCESS 0
#define FAILURE -1






class interrupt_flag
{
  std::atomic_bool b;
public:
  void set(){
    b = true;
  }
  bool is_set() const{
    return b;
  }

};
thread_local interrupt_flag this_thread_interrupt_flag;

class interruptible_thread
{
  std::thread internal_thread;
  interrupt_flag* flag;
public:
  template<typename FunctionType>
  interruptible_thread(FunctionType f)
  {
    try{
      std::promise<interrupt_flag*> p;

      internal_thread=std::thread([f,&p]{
	  p.set_value(&this_thread_interrupt_flag);
	  f();
	});
      flag=p.get_future().get();
    }
    catch(...){
      fprintf(stderr,"Caught something!\n");
    }
  }
  void interrupt()
  {
    if(flag)
      {
	
	flag->set();
      }
  }
};



void interruption_point()
{
  if(this_thread_interrupt_flag.is_set())
    {
      throw std::runtime_error("Hello");
    }
}



void foo()
{
  int done = 0;
  while(!done)
    {
      interruption_point();
    }
}


int verbose = 0;

#define Version "0.1"

static ArgOption args[] = {
  // Kind, 	  Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
static ArgDefs argp = { args, "s9", Version, NULL };

using namespace std;

int main(int argc, char* argv[]){
  progname = argv[0];
  ArgParser* ap = createArgumentParser(&argp);
  int result = parseArguments(ap, argc, argv);
  if(result){
    die("Error parsing arguments");
  }
  freeCommandLine();
  freeArgumentParser(ap);
  interruptible_thread *f = new interruptible_thread(&foo);
  f->interrupt();

  return SUCCESS;
}
