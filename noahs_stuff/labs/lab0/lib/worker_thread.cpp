#include "worker_thread.h"

worker_thread::worker_thread(play& cplay, const std::string& cname, std::fstream& cfile) :
  cur_play(cplay),
  char_name(cname),
  char_file(cfile){
  me = NULL;
  //assumed to already be open
}


int worker_thread::parseLine(std::string l){
  int lnum;
  std::stringstream ss(l);
  
  ss >> lnum;
  if(!ss.good()){
    return BAD_LINE;
  }

  while(ss.peek() == ' '){
    ss.ignore(1, ' ');
  }
  std::string ltext;
  if(!getline(ss, ltext, '/')){
    return BAD_LINE;
  }

  line new_line(lnum, this->char_name, ltext);
  this->cur_play<<new_line;
  return GOOD_LINE;
}

void worker_thread::readFile(){
  std::string l;
  while(getline(this->char_file, l, '\n')){
    //right now no need for return
    parseLine(l);
  }
  assert(this->char_file.eof());
}

void worker_thread::start(){
  this->me = new std::thread(&worker_thread::readFile, this);
}

void worker_thread::stop(){
  this->me->join();
}


