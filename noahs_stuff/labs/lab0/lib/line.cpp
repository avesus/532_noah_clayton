#include "line.h"

line::line(int lnum, std::string cname, std::string ltext){
  line_num = lnum;
  char_name = cname;
  line_text = ltext;
}

int line::getLineNum(){
  return this->line_num;
}

std::string line::getCharName(){
  return this->char_name;
}

std::string line::getText(){
  return this->line_text;
}

int line::isUnset(){
  return ((this->line_num == 0) && (this->char_name.empty()) && (this->line_text.empty()));
}
