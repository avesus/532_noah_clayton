#include "aint.h"
#include <stdio.h>
class a_enc : public int_class {
  char key;
 public:
 a_enc() : int_class() {
    key = 0x9;
  }
 a_enc(char new_key) : int_class() {
    key = new_key;
  }
  void operator() (char& a) {
    a ^= key;
  }
};
