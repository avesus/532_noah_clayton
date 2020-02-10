#include "play.h"


play::play(std::string pname) : arr() {
  this->name = pname;
  this->max_line = 0;
}

std::string play::getName(){
  return this->name;
}

play& play::operator<<(line& l){
  try{
  int lindex = l.getLineNum();
  this->max_line = MAX(lindex, this->max_line);
  assert(this->arr[lindex].isUnset());
  this->arr[lindex] = l;
  }
  catch (const std::exception& e){
    std::cerr << "Error adding line" << std::endl;
    std::cerr << e.what() << std::endl;
    throw std::runtime_error("add line");
  }
  catch (...) {
    std::cerr << "Caught unknown exception adding line" << std::endl;
    throw std::runtime_error("add line");
  }

  return *this;
}

void play::print(std::ostream& os){
  std::string cur;
  os << this->getName() << std::endl;
  for(int i=0;i<=this->max_line;i++){
    if(this->arr[i].isUnset()){
      continue;
    }
    if(cur!=arr[i].getCharName()){
      if(!cur.empty()){
	os << "\n";
      }
      os << arr[i].getCharName() << "." << std::endl;
      cur = arr[i].getCharName();
    }
    os << arr[i].getText() << std::endl;
  }
}
