#include "play.h"


play::play(std::string pname) : arr() {
  this->name = pname;
  this->max_line = 0;
}

std::string play::getName(){
  return this->name;
}

play& play::operator<<(line& l){
  int lindex = l.getLineNum();
  this->max_line = MAX(lindex, this->max_line);
  assert(this->arr[lindex].isUnset());
  this->arr[lindex] = l;
}

void play::print(std::ostream& os){
  std::string cur;
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
