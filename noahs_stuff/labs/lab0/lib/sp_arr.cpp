#include "sp_arr.h"

template <class T>
sp_arr<T>::sp_arr(int msize, int narr){
  assert(narr && (msize>1));
  this->arr = (T**)mycalloc(narr, sizeof(T*));
  this->arr[0] = (T*)mycalloc(msize<<1, sizeof(T));
  this->min_size = ulog2(msize);
  this->num_arr = narr;
}

template <class T>
sp_arr<T>::sp_arr(int msize) : sp_arr(msize, BIG_ENOUGH) {}


template <class T>
sp_arr<T>::sp_arr() :   sp_arr(2, BIG_ENOUGH) {}




template <class T>
T& sp_arr<T>::operator[](const int& n){

  int x = ulog2(n>>this->min_size);
  int y = n & ((~(1<<(x+this->min_size+(x==0)))));
  if(this->arr[x] == NULL){
    int next_size = (1<<(this->min_size))<<x;
    T* toadd = (T*)calloc(next_size, sizeof(T));
    //this should probably be replaced with atomics.
    std::lock_guard<std::mutex> lock(this->m);
    if(this->arr[x] == NULL){
      this->arr[x] = toadd;
    }else{
      free(toadd);
    }
  }
  return this->arr[x][y];
}



template class sp_arr<int>;
template class sp_arr<line>;
