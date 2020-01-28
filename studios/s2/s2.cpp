#include "s2.h"


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
static ArgDefs argp = { args, "s2", Version, NULL };

#define arr_size 7
int arr[7] = {-2, 19, 80, -47, 80, 80, -2};

struct r2d {
  int* arr;
  int n, m;
  r2d(int n, int m) : n(n), m(m) {
    arr = (int*)calloc(n*m, sizeof(int));
  }
  ~r2d(){

  }
  int* begin(){
    return this->arr;
  }

  int* end(){
    return this->arr + this->n*this->m;
  }


  int* operator[](int n) {
    return arr + n*this->m;
  }
  int sum(int n){
    int * temp = arr + n*this->m;
    int sink =0;
    for(int i=0;i<this->m;i++){
      sink+=temp[i];
    }
    return sink;
  }
};


using namespace std;


int sum_wrapper(r2d arr2, int * res, int i){
  res[i] = arr2.sum(i);
}
int main(int argc, char* argv[]){
  progname = argv[0];
  ArgParser* ap = createArgumentParser(&argp);
  int result = parseArguments(ap, argc, argv);
  if(result){
    die("Error parsing arguments");
  }
  r2d arr2(10, 5);
  for(int i=0;i<10;i++){
      for(int j=0;j<5;j++){
	arr2[i][j]=i;
      }
  }
  vector<thread> threads;
  int* res = (int*)calloc(10, sizeof(int));
  for(int i=0;i<10;i++){
    threads.push_back(thread(sum_wrapper,arr2,res, i));
  }

  for(int i=0;i<10;i++){
    threads[i].join();

  }
  for(int i=0;i<10;i++){
      printf("row[%d] -> %d\n", i, res[i]);
  }
  return SUCCESS;
}
