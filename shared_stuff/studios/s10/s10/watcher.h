#ifndef __WATCHER_H__
#define __WATCHER_H__

#include <string>
#include <atomic>
#include <sstream>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <iterator>
#include <map>
#include <unordered_map>


enum event_type{
  CALLING = 0,
  CALLED = 1,
  RETURNING = 2,
  RETURNED = 3,
  ALLOCING = 4,
  FREED = 5,
  MUTEX_ACQUIRING = 6,
  MUTEX_ACQUIRED = 7,
  MUTEX_RELEASING = 8,
  MUTEX_RELEASED = 9,
  THREAD_WAITING = 10,
  THREAD_WAKUP = 11,
  THREAD_NOTIFY_ONE = 12,
  THREAD_NOTIFY_ALL = 13,
  UNKNOWN = 14
};
#define NUM_EVENTS 15
#define EV_STR_LEN 32
const char event_strings[NUM_EVENTS][EV_STR_LEN] = {
  "Calling",  //0
  "Called", //1
  "Returning", //2
  "Returned", //3
  "Allocing", //4
  "Freed", //5
  "Mutex Acquiring", //6
  "Mutex Acquired", //7
  "Mutex Releasing", //8
  "Mutex Released", //9
  "Thread Waiting", //10
  "Thread Wakeup", //11
  "Thread Notify One", //12
  "Thread Notify All", //13
  "Unknown" //14
};


#define EV_MSG_LEN 256



struct event_info{
  unsigned long tid;
  void * obj;
  void * func;
  void * other;
  char * file;
  int line;
  event_type ev;
  event_info() {
    this->tid = 0;
    this->obj = NULL;
    this->func = NULL;
    this->other = NULL;
    this->file = NULL;
    this->line = 0;
    this->ev = UNKNOWN;
  }

};
  
typedef struct ev_map{
  int counter_max;
  int counter_cur;
  struct event_info * events;
}ev_map;

class event_logger;
std::ostream& operator<<(std::ostream& os, const event_type ev);
std::ostream& operator<<(std::ostream& os, const event_info ei);
std::ostream& operator<<(event_logger& el, const event_info ei);

class event_logger{
  std::unordered_map<void *, std::string> logs;
  std::unordered_map<unsigned long, ev_map *> tids;
  std::mutex m;
 public:
  std::ostream& my_os;
 event_logger(std::ostream& os) : my_os(os) {}
  void new_tid(unsigned long tid, int ev_count){
    if(this->tids.find(tid) == this->tids.end()){
      ev_map * e = (ev_map *)calloc(1, sizeof(ev_map));
      e->counter_max = ev_count;
      e->counter_cur = 0;
      struct event_info * events = (struct event_info *)calloc(ev_count,
							       sizeof(struct event_info));
      e->events = events;
      this->tids[tid] = e;
    }
  }
  void new_tid(unsigned long tid){
    if(this->tids.find(tid) == this->tids.end()){
      ev_map * e = (ev_map *)calloc(1, sizeof(ev_map));
      e->counter_max = 1024;
      e->counter_cur = 0;
      struct event_info * events = (struct event_info *)calloc(1024,
							       sizeof(struct event_info));
      e->events = events;
      this->tids[tid] = e;
    }
  }
  void ev_add(unsigned long tid, struct event_info ei){
    if(this->tids.find(tid) != this->tids.end()){

      std::unordered_map<unsigned long, ev_map *>::iterator it = this->tids.find(tid);
      if(it->second->counter_cur >= it->second->counter_max){
	return;
      }
      it->second->events[it->second->counter_cur++] = ei;
    }
  }
  void print(){
    std::unordered_map<unsigned long, ev_map *>::iterator it = this->tids.begin();
    while(it != this->tids.end()){
      for(int i=0;i<it->second->counter_cur;i++){
	(*this) << it->second->events[i];
	std::cout << it->second->events[i];
      }
      it++;
    }
  }
  
  void _register(void * key, std::string val){
    std::lock_guard<std::mutex> lock(this->m);
    if(key != NULL){
      this->logs[key] = val;
    }
  }
  void unregister(void *key){
    std::lock_guard<std::mutex> lock(this->m);
    if(key != NULL){
      this->logs.erase(key);
    }
  }
  std::string get_val(void *key){
    std::lock_guard<std::mutex> lock(this->m);
    if(key != NULL){
      if(this->logs.find(key) != this->logs.end()){
	return this->logs.find(key)->second;
      }
      else {
	std::string empty_str;
	return empty_str;
      }
    }
    std::string empty_str;
    return empty_str;
  }
};




#endif
