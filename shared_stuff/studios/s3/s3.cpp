#include "s3.h"


#define SUCCESS 0
#define FAILURE -1


int verbose = 0;
int inserts = 0;
int nthreads = 0;
#define Version "0.1"

static ArgOption args[] = {
  // Kind, 	  Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindOption,   Integer, 		"-t", 	    0,     &nthreads, 		"Set number of threads" },
  { KindOption,   Integer, 		"--inserts", 0,     &inserts, 		"Set number of inserts per thread" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
static ArgDefs argp = { args, "s3", Version, NULL };

using namespace std;

template <class T> 
class vwrapper {
  vector<int> v;
  T m;
public:
  vwrapper* operator<< (const int& a) {
    lock_guard<T> lock(this->m);
    this->v.push_back(a);
    //had an issue with mutex and returning object seems that going
    //out of scope would call delete on mutex which obviously is big
    //nono so changed to ptr
    return this;
  }
  int operator() () {
    lock_guard<T> lock(this->m);
    return this->sum();
  }
  
  int sum(){
    int sum = 0;
    for(std::vector<int>::iterator it = this->v.begin();
	it != this->v.end();
	++it){
      sum += *it;
    }
    return sum;
  }
  
  void print(const char* msg, ...){
    lock_guard<T> lock(this->m);
    va_list ap;
    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end (ap);
  }
  unsigned long factorial_of_sum(){
    lock_guard<T> lock(this->m);
    int sum = this->sum();
    unsigned long fact = 1;
    for(int i=1;i<=sum;i++){
      fact *= i;
    }
    return fact;
  }
  thread new_thread(int tid){
    //this was a serious RTFM moment
    return thread(&vwrapper::doSum, this, tid);
  }
  
  void doSum(int tid){
    for(int i=0;i<inserts;i++){
      if(verbose){
	this->print("Tid[%d] -> pushing back %d\n", tid, i);
      }
      (*this) << i;
    }
    if(verbose){
      this->print("Tid[%d] -> sum %d\n", tid, (*this)());
      //this is bound to overflow
      this->print("Tid[%d] -> fact %lu\n", tid, this->factorial_of_sum());
    }
  }
  
};





int main(int argc, char* argv[]){
  progname = argv[0];
  ArgParser* ap = createArgumentParser(&argp);
  int result = parseArguments(ap, argc, argv);
  if(result){
    die("Error parsing arguments");
  }
  freeCommandLine();
  freeArgumentParser(ap);
  if(inserts&&nthreads){
    vector<thread> threads;
    vwrapper<recursive_mutex> vw;
    for(int i=0;i<nthreads;i++){
      threads.push_back(vw.new_thread(i));
    }
    for(int i=0;i<nthreads;i++){
      threads[i].join();
    }
  }

  return SUCCESS;
}
