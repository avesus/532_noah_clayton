#ifndef _MY_THREAD_H_
#define _MY_THREAD_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include "line.h"
#include "play.h"

#define BAD_LINE 0
#define GOOD_LINE 1
#define PARSE_ERROR 2
#define OTHER_ERROR 3

class worker_thread{
  play& cur_play;
  const std::string& char_name;
  std::fstream& char_file;
  std::thread* me;
 public:
  worker_thread(play& cplay, const std::string& cname, std::fstream& cfile);
  int parseLine(std::string l);
  void readFile();
  void start();
  void stop();
};

#endif
