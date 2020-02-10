#include "opt.h"


unsigned int ulog2(unsigned int n){
  unsigned int s, t;
  t = (n > 0xffff) << 4; n >>= t;
  s = (n > 0xff  ) << 3; n >>= s, t |= s;
  s = (n > 0xf   ) << 2; n >>= s, t |= s;
  s = (n > 0x3   ) << 1; n >>= s, t |= s;
  return (t | (n >> 1));
}
