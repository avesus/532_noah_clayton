#include "s5.h"


#define SUCCESS 0
#define FAILURE -1


int verbose = 0;
int inserts = 1;
#define Version "0.1"

static ArgOption args[] = {
  // Kind, 	  Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindOption,   Integer, 		"-i",       0,     &inserts, 		"Set verbosity level" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
static ArgDefs argp = { args, "s5", Version, NULL };

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
  active_object<int> act_obj_base;
  active_object<int> act_obj(&act_obj_base);
  act_obj_base.activate();

  for(int i=0;i<inserts;i++){
    act_obj.enque(i);
  }
  for(int i=0;i<100;i++){
  act_obj.activate();
  usleep(1000);
  act_obj.deactivate();
  }
  fprintf(stderr,"Complete!\n");

  
  return SUCCESS;
}
