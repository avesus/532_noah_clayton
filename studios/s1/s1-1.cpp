#include "s1-1.h"


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
static ArgDefs argp = { args, "s1-1", Version, NULL };

using namespace std;


void printOut(int test){
  cout << "hello world! " << test << endl;
}
int main(int argc, char* argv[]){
  progname = argv[0];
  ArgParser* ap = createArgumentParser(&argp);
  int result = parseArguments(ap, argc, argv);
  if(result){
    die("Error parsing arguments");
  }
  vector<thread> threads;
  for(int i=0;i<20;i++){
  threads.push_back(thread(printOut, 43534));
  }
  for(int i=0;i<threads.size();i++){
    threads[i].join();
  }
  
  return SUCCESS;
}
