#include "sync_list.h"

template <class T>
sync_list<T>::sync_list() : l() {
  ubound = 0;
  lbound = 0;
}

template <class T>
sync_list<T>::sync_list(int u, int l) : sync_list() {
  ubound = u;
  lbound = l;
}

template <class T>
void sync_list<T>::push_back(T val){
  if(this->ubound){
    std::unique_lock<std::mutex> lock(this->m);
    while(this->l.size() > this->ubound){
      cv_push.wait(lock);
    }
    int to_notify = this->l.size() < this->lbound;
    this->l.push_back(val);
    if(to_notify){
      this->cv_pop.notify_one();
    }
  }else{
    std::lock_guard<std::mutex> lock(this->m);
    int to_notify = this->l.empty();
    this->l.push_back(val);
    if(to_notify){
      this->cv_pop.notify_one();
    }
  }
}




template <class T>
T sync_list<T>::pop_back(){
  if(this->lbound){
    std::unique_lock<std::mutex> lock(this->m);
    while(this->l.size() < this->lbound){
      cv_pop.wait(lock);
    }
    int to_notify = this->l.size() > this->ubound;
    T ret = this->l.back();
    this->l.pop_back();
    lock.unlock();
    if(to_notify){
      this->cv_push.notify_one();
    }
    return ret;
  }else{
    std::unique_lock<std::mutex> lock(this->m);
    while(this->l.empty()){
      cv_pop.wait(lock);
    }
    T ret = this->l.back();
    this->l.pop_back();
    lock.unlock();
    return ret;
  }
}

template <class T>
void sync_list<T>::push_front(T val){
  if(this->ubound){
    std::unique_lock<std::mutex> lock(this->m);
    while(this->l.size() > this->ubound){
      cv_push.wait(lock);
    }
    int to_notify = this->l.size() < this->lbound;
    this->l.push_front(val);
    if(to_notify){
      this->cv_pop.notify_one();
    }
  }else{
    std::lock_guard<std::mutex> lock(this->m);
    int to_notify = this->l.empty();
    this->l.push_front(val);
    if(to_notify){
      this->cv_pop.notify_one();
    }
  }
}


template <class T>
T sync_list<T>::pop_front(){
  if(this->lbound){
    std::unique_lock<std::mutex> lock(this->m);
    while(this->l.size() < this->lbound){
      cv_pop.wait(lock);
    }
    int to_notify = this->l.size() > this->ubound;
    T ret = this->l.back();
    this->l.pop_back();
    lock.unlock();
    if(to_notify){
      this->cv_push.notify_one();
    }
    return ret;
  }else{
    std::unique_lock<std::mutex> lock(this->m);
    while(this->l.empty()){
      cv_pop.wait(lock);
    }
    T ret = this->l.front();
    this->l.pop_front();
    lock.unlock();
    return ret;
  }
}



template class sync_list<int>;
