#include "s7.h"


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
static ArgDefs argp = { args, "s7", Version, NULL };

using namespace std;
//#define S_2
//#define S_3
#define S_12


#ifdef S_2
template<typename T>
class lock_free_stack
{
private:
  struct node
  {
    T data;
    node* next;
    node(T const& data_):
      data(data_)
    {}
  };
  std::atomic<node*> head;
public:
  void push(T const& data)
  {
    node* const new_node=new node(data);
    new_node->next=head.load();
    while(!head.compare_exchange_weak(new_node->next,new_node));
  }
  void pop(T& result)
  {
    node* old_head=head.load();
    while(!head.compare_exchange_weak(old_head,old_head->next));
    result=old_head->data;
  }  
};
#endif
#ifdef S_3
template<typename T>
class lock_free_stack
{
private:
  struct node
  {
    std::shared_ptr<T> data;
    node* next;
    node(T const& data_):
      data(std::make_shared<T>(data_))
    {}
  };
  std::atomic<node*> head;
public:
  void push(T const& data)

  {
    node* const new_node=new node(data);
    new_node->next=head.load();
    while(!head.compare_exchange_weak(new_node->next,new_node));
  }
  std::shared_ptr<T> pop()
  {
    node* old_head=head.load();
    while(old_head &&
	  !head.compare_exchange_weak(old_head,old_head->next));
    return old_head ? old_head->data : std::shared_ptr<T>();
  }
};
#endif
#ifdef S_12
template<typename T>
class lock_free_stack
{
private:
  struct node;
  struct counted_node_ptr
  {
    int external_count;
    node* ptr;
  };
  struct node
  {
    std::shared_ptr<T> data;
    std::atomic<int> internal_count;
    counted_node_ptr next;
    node(T const& data_):
      data(std::make_shared<T>(data_)),
      internal_count(0)
    {}
  };
  std::atomic<counted_node_ptr> head;
  void increase_head_count(counted_node_ptr& old_counter)
  {
    counted_node_ptr new_counter;
    do
      {
	new_counter=old_counter;
	++new_counter.external_count;
      }
    while(!head.compare_exchange_strong(old_counter,new_counter,
					std::memory_order_acquire,
					std::memory_order_relaxed));
    old_counter.external_count=new_counter.external_count;
  }
public:
  ~lock_free_stack()
  {
    while(pop());
  }
  void push(T const& data)
  {
    counted_node_ptr new_node;
    new_node.ptr=new node(data);
    new_node.external_count=1;
    new_node.ptr->next=head.load(std::memory_order_relaxed);
    while(!head.compare_exchange_weak(new_node.ptr->next,new_node,
				      std::memory_order_release,
				      std::memory_order_relaxed));
  }
  std::shared_ptr<T> pop()
  {
    counted_node_ptr old_head=
      head.load(std::memory_order_relaxed);
    for(;;)
      {
	increase_head_count(old_head);
	node* const ptr=old_head.ptr;
	if(!ptr)
	  {
	    return std::shared_ptr<T>();
	  }
	if(head.compare_exchange_strong(old_head,ptr->next,
					std::memory_order_relaxed))
	  {
	    std::shared_ptr<T> res;
	    res.swap(ptr->data);
	    int const count_increase=old_head.external_count-2;
	    if(ptr->internal_count.fetch_add(count_increase,
					     std::memory_order_release)==-count_increase)
	      {
		delete ptr;
	      }
	    return res;
	  }
	else if(ptr->internal_count.fetch_add(-1,
					      std::memory_order_relaxed)==1)
	  {
	    ptr->internal_count.load(std::memory_order_acquire);
	    delete ptr;
	  }
      }
  }
};
#endif
/* Serendipity had be accidentally copy this one
   template<typename T>
   class lock_free_queue
   {
   private:
   void set_new_tail(counted_node_ptr &old_tail,
   counted_node_ptr const &new_tail)
   {
   node* const current_tail_ptr=old_tail.ptr;
   while(!tail.compare_exchange_weak(old_tail,new_tail) &&
   old_tail.ptr==current_tail_ptr);
   if(old_tail.ptr==current_tail_ptr)
   free_external_counter(old_tail);
   else
   current_tail_ptr->release_ref();
   }
   public:
   void push(T new_value)
   {
   std::unique_ptr<T> new_data(new T(new_value));
   counted_node_ptr new_next;
   new_next.ptr=new node;
   new_next.external_count=1;
   counted_node_ptr old_tail=tail.load();
   for(;;)
   {
   increase_external_count(tail,old_tail);
   T* old_data=nullptr;
   if(old_tail.ptr->data.compare_exchange_strong(
   old_data,new_data.get()))
   {
   counted_node_ptr old_next={0};
   if(!old_tail.ptr->next.compare_exchange_strong(
   old_next,new_next))
   {
   delete new_next.ptr;
   new_next=old_next;
   }
   set_new_tail(old_tail, new_next);
   new_data.release();
   break;
   }
   else
   {
   counted_node_ptr old_next={0};
   if(old_tail.ptr->next.compare_exchange_strong(
   old_next,new_next))
   {
   old_next=new_next;
   new_next.ptr=new node;
   }
   set_new_tail(old_tail, old_next);
   }
   }
   }
   };
*/
void push_pop(vector<int>* container, lock_free_stack<int>* stack, int offset){
  for(int i=offset;i<runs;i+=2){
    stack->push(i);
  }
  for(int i=offset;i<runs;i+=2){
    container->push_back(*stack->pop());
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
  lock_free_stack<int>* stack = new lock_free_stack<int>;
  vector<int>* thr_0_pops = new vector<int>;
  vector<int>* thr_1_pops = new vector<int>;

  thread thr_0 = thread(push_pop, thr_0_pops, stack, 0);
  thread thr_1 = thread(push_pop, thr_1_pops, stack, 1);
  thr_0.join();
  thr_1.join();
  assert(thr_0_pops->size() == thr_1_pops->size());
  printf("Thread 0:");
  for(int i=0;i<thr_0_pops->size();i++){
    printf(" %d", (*thr_0_pops)[i]);
  }
  printf("\nThread 1:");
  for(int i=0;i<thr_1_pops->size();i++){
    printf(" %d", (*thr_1_pops)[i]);
  }
  printf("\n");

  return SUCCESS;
}
