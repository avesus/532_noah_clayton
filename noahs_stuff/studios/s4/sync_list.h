#ifndef _SYNC_LIST_H_
#define _SYNC_LIST_H_

#include <list>
#include <mutex>
#include <condition_variable>

template<class T>
class sync_list{
  std::list<T> l;
  int ubound;
  int lbound;
  std::mutex m;
  std::condition_variable cv_pop;
  std::condition_variable cv_push;
 public:
  sync_list();
  sync_list(int u, int l);
  void push_back(T val);
  T pop_back();
  void push_front(T val);
  T pop_front();

};

#endif
