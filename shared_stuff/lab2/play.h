#pragma once

#include <mutex>
#include <shared_mutex>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <condition_variable>
#include <fstream>
#include <set>

#include "line.h"

using namespace std;


enum {
  success,
  cant_open_file,
  join_exception
};


class Play {
  const vector<string> &names;
  vector<string>::const_iterator it;
  uint32_t line_counter;
  int32_t scene_fragment_counter;
  int32_t n_chars;
  int32_t n_passed;
  string cur_char;

  mutex m;
  condition_variable cv;

 public:

 Play(const vector<string> &names): names(names), it(names.begin()), line_counter(1), scene_fragment_counter(1), n_chars(0), n_passed(0) {}
  

  void recite(set<line>::iterator& it, int32_t expec_fragment_lines);

  void set_n_chars(int32_t n) {
    n_chars = n;
  }

  void player_exit() {
    lock_guard<mutex> l(m);
    if (n_passed == --n_chars && n_chars > 0) {
      // nobody could read this line, need to skip it
      cerr << endl << "****** line " << line_counter << " skipped ******";
      line_counter++;
      n_passed = 0;
      cv.notify_all();
    }
  }

};

