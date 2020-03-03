#include "s11.h"

#define print_bool(X) (X) ? "true" : "false"

bool isPrime(uint n){
  if(n > 2 && (n%2) == 0){
    return false;
  }
  int sqrt_n = sqrt((double)n) + 1;
  for(int i=3; i<sqrt_n; i+=2){
    if((n%i) == 0){
      return false;
    }
  }
  return true;
}

void sync_que::push_num(const uint &v){
  std::lock_guard<std::mutex> lock(this->m);
  this->que.push(v);
  cv.notify_all();
}

uint sync_que::pop_num(int tid){
  std::unique_lock<std::mutex> lock(this->m);
  cv.wait(lock, [&] { return (this->que.size()); });
  if(this->leader_tid != tid){
    char buf[16] = "";
    sprintf(buf, "Leader: %d -> %d", leader_tid, tid);
    this->leader_tid = tid;
    this->sp->print(std::string(buf));
  }
  uint ret = this->que.front();
  this->que.pop();
  return ret;
}


void worker_thread::do_work(){
  char buf[16] = "";
  while(!done){
    uint got = this->sq->pop_num(this->my_tid);
    sprintf(buf, "%d -> %s", got, print_bool(isPrime(got)));
    this->sp->print(std::string(buf));
  }
}

void worker_thread::start(){
  this->me = new std::thread(&worker_thread::do_work, this);
}

void worker_thread::stop(){
  this->done = 1;
  this->me->join();
}


void sync_printer::print(std::string to_print){
  std::lock_guard<std::mutex> lock(this->m);
  this->my_os << to_print << std::endl;
}


#define SUCCESS 0
#define FAILURE -1



int verbose = 0;
int check_to = 1;
int nthreads = 8;
#define Version "0.1"

static ArgOption args[] = {
  // Kind, 	  Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindOption,   Integer, 		"-p", 	    0,     &check_to, 		"Check 1 to p" },
  { KindOption,   Integer, 		"-t", 	    0,     &nthreads, 		"Number of threads" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
static ArgDefs argp = { args, "s11", Version, NULL };

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


  sync_printer *s = new sync_printer(std::cout);
  sync_que *q = new sync_que(s);
  vector<worker_thread *> workers;
  for(int i=0; i<nthreads; i++){
    worker_thread * w = new worker_thread(q, s, i);
    workers.push_back(w);
  }
  for(int i=0; i<nthreads; i++){
    workers[i]->start();
  }
  int done = 0;
  while(!done){
    int temp;
    cin >> temp;
    if(temp == -1){
      return SUCCESS;
    }else{
      q->push_num(temp);
    }
  }
  for(int i=0; i<nthreads; i++){
    workers[i]->stop();
  }
  
  return SUCCESS;
}
