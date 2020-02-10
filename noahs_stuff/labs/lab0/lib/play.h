#ifndef _PLAY_H_
#define _PLAY_H_

#include "sp_arr.h"
#include <iostream>
#include <sstream>
#include <mutex>
#include <assert.h>
#include "../helpers/opt.h"




class play{
  std::string name;
  sp_arr<line> arr; 
  int max_line;
 public:
  std::string getName();
  play(std::string pname);
  play& operator<<(line& l);
  void print(std::ostream& os);
};

#endif
