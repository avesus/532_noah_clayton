#include "aint.h"
#include <stdio.h>
class pass_through : public int_class {
 public:
  void operator() (char&) {

  }
};
