#ifndef _SP_ARR_H_
#define _SP_ARR_H_

#include "../helpers/util.h"
#include <mutex>
#include <assert.h>
#include "line.h"

#define BIG_ENOUGH 32


template <class T>
class sp_arr{
  int min_size;
  int num_arr;
  T** arr;
  std::mutex m;
 public:
  sp_arr(int msize, int narr);
  sp_arr(int msize);
  sp_arr();
  //  ~sp_arr();
  T& operator[](const int& n);
};
 

#endif
