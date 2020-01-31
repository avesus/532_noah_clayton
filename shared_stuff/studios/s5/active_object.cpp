#include "active_object.h"

template <class T>
active_object<T>::active_object(): sync_list() {
  to_run = false;
  next = NULL;
}

template <class T>
active_object<T>::active_object(active_object* next_obj): active_object() {
  next = next_obj;
}

template <class T>
void active_object<T>::enque(T t){
  this->sync_list.push_front(t);
}


template <class T>
void active_object<T>::activate(){
  if(this->to_run == false){
    this->to_run = true;
    thread t = std::thread(&active_object::run_service, this);
    t.detach();
  }
}


template <class T>
void active_object<T>::deactivate(){
  if(this->to_run == true){
    this->to_run = false;
    this->sync_list.shut_down();
  }
}

template <class T>
void active_object<T>::run_service(){
  while(this->to_run == true){
    T ret = this->sync_list.pop_back();
    if(this->next){
      this->next->enque(ret);
    }
    else{
      cout << ret << endl;
    }
  }
  return;
}


template class active_object<int>;
