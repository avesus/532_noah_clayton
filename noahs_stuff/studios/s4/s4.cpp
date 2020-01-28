#include "s4.h"


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

static ArgDefs argp = { args, "s4", Version, NULL };

using namespace std;



#define nthreads 4


void pusher_popper(sync_list<int>* sl){
 for(int i=0;i<100;i++){
    if(i&1){
      sl->push_back(i);
    }else{
      sl->push_front(i);
    }
  }
   for(int i=0;i<50;i++){
    cout << sl->pop_front() << endl;
  }
  for(int i=0;i<50;i++){
    cout << sl->pop_back() << endl;
  }
}
int main(int argc, char* argv[]){
  progname = argv[0];
  ArgParser* ap = createArgumentParser(&argp);
  int result = parseArguments(ap, argc, argv);
  if(result){
    die("Error parsing arguments");
  }
  freeCommandLine();
  freeArgumentParser(ap);
  sync_list<int> sl(25, 5);
  vector<thread> t;
  for(int i=0;i<nthreads;i++){
    t.push_back(thread(pusher_popper, &sl));
  }
  for(int i=0;i<nthreads;i++){
    t[i].join();
  }

  return SUCCESS;
}
