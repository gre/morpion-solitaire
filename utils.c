#include "utils.h"

int util_abs(int n) {
  return n<0 ? -n : n;
}

int util_inRangeExclusif(int i, int a, int b) {
  int tmp;
  if(a>b) {
    tmp = a;
    a = b;
    b = tmp;
  }
  return a<i && i<b;
}
