#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <atomic>
#include <sstream>
#include <vector>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <iterator>
#include <map>
#include <queue>
#include "helpers/locks.h"
#include "helpers/temp.h"
#include "helpers/opt.h"
#include "helpers/util.h"
#include "helpers/arg.h"
#include "helpers/bits.h"
#include "helpers/timing.h"

typedef uint uint32_t;

class sync_printer {
  std::mutex m;
  std::ostream &my_os;

 public:
 sync_printer(std::ostream &os): my_os(os) {};
  void print(std::string to_print);
};


class sync_que {
  std::queue<uint> que;
  std::mutex m;
  std::condition_variable cv;
  sync_printer * sp;
  int leader_tid;
 public:
  sync_que(sync_printer *s): leader_tid(0) {
    sp = s;
  }
  void push_num(const uint &v);
  uint pop_num(int tid);
};

class worker_thread {
  unsigned long done;
  std::thread* me;
  int my_tid;
  sync_printer * sp;
  sync_que * sq;
 public:
 worker_thread(sync_que *q, sync_printer *s, int tid): done(0), my_tid(tid) {
    sp = s;
    sq = q;
  }
  
  void do_work();
  void start();
  void stop();
};


