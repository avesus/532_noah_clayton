#ifndef _ACTIVE_OBJECT_H_
#define _ACTIVE_OBJECT_H_
#include "synchronized_list.h"

template<class T>
class active_object {
  synchronized_list<T> sync_list;
  bool to_run;
  void run_service();
  active_object* next;
 public:
  active_object();
  active_object(active_object* next_obj);
  void activate();
  void deactivate();
  void enque(T t);

  


};



#endif //_ACTIVE_OBJECT_H_
