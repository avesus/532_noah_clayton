#ifndef _SP_ARR_H_
#define _SP_ARR_H_

#include "../helpers/opt.h"
#include <mutex>
#include <assert.h>
#include "line.h"

#define BIG_ENOUGH 32
#define ALLOC_ERROR -1

//basically this is a resizing array that resizes in place to avoid
//having to lock multiple threads. beneath the surface it functions as
//a 2d array of 1d arrays growing by powers of 2. This allows for
//quickly mappying 1d indexing to 2d indexes. It also allows for
//resizing by just allocating the next index in the outside array.
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
  ~sp_arr();
  T& operator[](const int& n);
};
 

#endif
