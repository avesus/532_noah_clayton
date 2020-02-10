#include "s6.h"


#define SUCCESS 0
#define FAILURE -1


int verbose = 0;
int runs = 1;
#define Version "0.1"

static ArgOption args[] = {
  // Kind, 	  Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindOption,   Integer, 		"--runs",   0,     &runs, 		"Set num runs" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
static ArgDefs argp = { args, "s6", Version, NULL };

std::atomic_bool atm_bool; 
std::atomic_char atm_char; 
std::atomic_schar atm_schar; 
std::atomic_uchar atm_uchar;
std::atomic_int atm_int; 
std::atomic_uint atm_uint; 
std::atomic_short atm_short; 
std::atomic_ushort atm_ushort;
std::atomic_long atm_long;
std::atomic_ulong atm_ulong;
std::atomic_llong atm_llong;
std::atomic_ullong atm_ullong;
std::atomic_char16_t atm_char16_t;
std::atomic_char32_t atm_char32_t;
std::atomic_wchar_t atm_wchar_t;
//str atm_bool, atm_char, atm_schar,  atm_uchar, atm_int, atm_uint, atm_short, atm_ushort, atm_long, atm_ulong, atm_llong, atm_ullong, atm_char16_t, atm_char32_t, atm_wchar_t, 


std::atomic_int_least8_t atm_int_least8_t; 
std::atomic_uint_least8_t atm_uint_least8_t; 
std::atomic_int_least16_t atm_int_least16_t; 
std::atomic_uint_least16_t atm_uint_least16_t; 
std::atomic_int_least32_t atm_int_least32_t; 
std::atomic_uint_least32_t atm_uint_least32_t; 
std::atomic_int_least64_t atm_int_least64_t; 
std::atomic_uint_least64_t atm_uint_least64_t; 
std::atomic_int_fast8_t atm_int_fast8_t; 
std::atomic_uint_fast8_t atm_uint_fast8_t; 
std::atomic_int_fast16_t atm_int_fast16_t; 
std::atomic_uint_fast16_t atm_uint_fast16_t; 
std::atomic_int_fast32_t atm_int_fast32_t; 
std::atomic_uint_fast32_t atm_uint_fast32_t; 
std::atomic_int_fast64_t atm_int_fast64_t; 
std::atomic_uint_fast64_t atm_uint_fast64_t; 
std::atomic_intptr_t atm_intptr_t; 
std::atomic_uintptr_t atm_uintptr_t; 
std::atomic_size_t atm_size_t; 
std::atomic_ptrdiff_t atm_ptrdiff_t; 
std::atomic_intmax_t atm_intmax_t; 
std::atomic_uintmax_t atm_uintmax_t; 
using namespace std;


//#define MEM_ORDER std::memory_order_relaxed
//#define MEM_ORDER std::memory_order_seq_cst
#define MEM_ORDER std::memory_order_acq_rel
std::atomic<bool> x,y;
std::atomic<int> z;

void write_x_then_y() {
 x.store(true,MEM_ORDER);
 y.store(true,MEM_ORDER);
}

void read_y_then_x() {
  while(!y.load(MEM_ORDER));
  if(x.load(MEM_ORDER))
    ++z;
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
  printf("atomic_bool.is_lock_free() ->  %d\natomic_char.is_lock_free() ->  %d\natomic_schar.is_lock_free() ->  %d\natomic_uchar.is_lock_free() ->  %d\natomic_int.is_lock_free() ->  %d\natomic_uint.is_lock_free() ->  %d\natomic_short.is_lock_free() ->  %d\natomic_ushort.is_lock_free() ->  %d\natomic_long.is_lock_free() ->  %d\natomic_ulong.is_lock_free() ->  %d\natomic_llong.is_lock_free() ->  %d\natomic_ullong.is_lock_free() ->  %d\natomic_char16_t.is_lock_free() ->  %d\natomic_char32_t.is_lock_free() ->  %d\natomic_wchar_t.is_lock_free() ->  %d\n", atm_bool.is_lock_free(), atm_char.is_lock_free(), atm_schar.is_lock_free(),  atm_uchar.is_lock_free(), atm_int.is_lock_free(), atm_uint.is_lock_free(), atm_short.is_lock_free(), atm_ushort.is_lock_free(), atm_long.is_lock_free(), atm_ulong.is_lock_free(), atm_llong.is_lock_free(), atm_ullong.is_lock_free(), atm_char16_t.is_lock_free(), atm_char32_t.is_lock_free(), atm_wchar_t.is_lock_free());
  printf("atomic_int_least8_t.is_lock_free() -> %d\natomic_uint_least8_t.is_lock_free() -> %d\natomic_int_least16_t.is_lock_free() -> %d\natomic_uint_least16_t.is_lock_free() -> %d\natomic_int_least32_t.is_lock_free() -> %d\natomic_uint_least32_t.is_lock_free() -> %d\natomic_int_least64_t.is_lock_free() -> %d\natomic_uint_least64_t.is_lock_free() -> %d\natomic_int_fast8_t.is_lock_free() -> %d\natomic_uint_fast8_t.is_lock_free() -> %d\natomic_int_fast16_t.is_lock_free() -> %d\natomic_uint_fast16_t.is_lock_free() -> %d\natomic_int_fast32_t.is_lock_free() -> %d\natomic_uint_fast32_t.is_lock_free() -> %d\natomic_int_fast64_t.is_lock_free() -> %d\natomic_uint_fast64_t.is_lock_free() -> %d\natomic_intptr_t.is_lock_free() -> %d\natomic_uintptr_t.is_lock_free() -> %d\natomic_size_t.is_lock_free() -> %d\natomic_ptrdiff_t.is_lock_free() -> %d\natomic_intmax_t.is_lock_free() -> %d\natomic_uintmax_t.is_lock_free() -> %d\n", atm_int_least8_t.is_lock_free(), atm_uint_least8_t.is_lock_free(), atm_int_least16_t.is_lock_free(), atm_uint_least16_t.is_lock_free(), atm_int_least32_t.is_lock_free(), atm_uint_least32_t.is_lock_free(), atm_int_least64_t.is_lock_free(), atm_uint_least64_t.is_lock_free(), atm_int_fast8_t.is_lock_free(), atm_uint_fast8_t.is_lock_free(), atm_int_fast16_t.is_lock_free(), atm_uint_fast16_t.is_lock_free(), atm_int_fast32_t.is_lock_free(), atm_uint_fast32_t.is_lock_free(), atm_int_fast64_t.is_lock_free(), atm_uint_fast64_t.is_lock_free(), atm_intptr_t.is_lock_free(), atm_uintptr_t.is_lock_free(), atm_size_t.is_lock_free(), atm_ptrdiff_t.is_lock_free(), atm_intmax_t.is_lock_free(), atm_uintmax_t.is_lock_free());



 z=0;

 for(int i=0;i<runs;i++){
   x=false;
   y=false;

   std::thread a(write_x_then_y);
   std::thread b(read_y_then_x);
   a.join();
   b.join();
 }
 assert(z.load()!=0);
 printf("z -> %d\n", z.load());
 return SUCCESS;
}
