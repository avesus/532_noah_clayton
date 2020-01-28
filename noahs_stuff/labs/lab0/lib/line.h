#ifndef _LINE_H_
#define _LINE_H_

#include <iostream>
#include <sstream>

class line{
  int line_num;
  std::string char_name;
  std::string line_text;
 public:
  int getLineNum();
  std::string getCharName();
  std::string getText();
  int isUnset();
  line(int lnum, std::string cname, std::string ltext);
};

#endif
