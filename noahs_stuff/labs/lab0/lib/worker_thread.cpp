#include "worker_thread.h"

worker_thread::worker_thread(play& cplay, const std::string& cname, std::fstream& cfile) :
  cur_play(cplay),
  char_name(cname),
  char_file(cfile){
  me = NULL;
  //assumed to already be open
}


int worker_thread::parseLine(std::string l){
  try{
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
    try {
      this->cur_play<<new_line;
    }
    catch (const std::exception& e){
      std::cerr << "Fatal adding new line" << std::endl;
      std::cerr << e.what() << std::endl;
      return OTHER_ERROR;
    }
    catch (...) {
      std::cerr << "Caught unknown exception adding line" << std::endl;
      return OTHER_ERROR;
    } 
    return GOOD_LINE;
  }
  catch (const std::exception& e){
    std::cerr << "Fatal error parsing line" << std::endl;
    std::cerr << e.what() << std::endl;
    return PARSE_ERROR;
  }
  catch (...) {
    std::cerr << "Caught unknown exception parsing line" << std::endl;
    return PARSE_ERROR;
  }
}

void worker_thread::readFile(){
  try {
    std::string l;
    while(getline(this->char_file, l, '\n')){
      int ret = parseLine(l);
      if(ret == PARSE_ERROR){
	throw std::runtime_error("parsing error");
      }
      if(ret == OTHER_ERROR){
	throw std::runtime_error("error");
      }
    }
    //probably unnecissary with try/catch
    assert(this->char_file.eof());
  }
  catch (const std::exception& e){
    std::cerr << "Error reading file" << std::endl;
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "Caught unknown exception reading file" << std::endl;
  }
}

void worker_thread::start(){
  try {
    this->me = new std::thread(&worker_thread::readFile, this);
  }
  catch (const std::exception& e){
    std::cerr << "Fatal error creating thread" << std::endl;
    std::cerr << e.what() << std::endl;
    return;
  }
  catch (...) {
    std::cerr << "Caught unknown exception in thread creation" << std::endl;
    return;
  }
}

void worker_thread::stop(){
  try {
    this->me->join();
  }
  catch (const std::exception& e){
    std::cerr << "Fatal error joining thread" << std::endl;
    std::cerr << e.what() << std::endl;
    return;
  }
  catch (...) {
    std::cerr << "Caught unknown exception in thread join" << std::endl;
    return;
  }
}


