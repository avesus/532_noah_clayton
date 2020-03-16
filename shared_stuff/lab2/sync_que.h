
#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <memory>
#include <condition_variable>
#include <mutex>

#include "helpers/opt.h"



using namespace std;

struct p_info {
  string name;
  size_t frag_num;
  string file;
  p_info() {}
};


class sync_que {

  mutex m;

  queue<p_info> que;
    
 public:
  condition_variable cv;
  volatile uint64_t done = 0;
  void push(p_info p);
  p_info pop();

  size_t size() {
    return que.size();
  }
};


