#include "utils.h"

double ys_sum(double *arr, int len) {
    double res=0;
    for (int i=0; i<len; ++i) {
        res += arr[i];
    }
    return res;
}


int int_find_arr(int* arr, int len, int val) {
  for(int i=0; i<len; ++i) {
    if (arr[i] == val){
      return i;
    }
  }
  return -1;
}
