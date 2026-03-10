/**
 * Author: caterpillow
 * License: CC0
 * Source: cp algorithms
 * Description: Gray codes 
 */
#pragma once

int g(int n) {
    return n ^ (n >> 1);
}

int rev_g (int g) {
  int n = 0;
  for (; g; g >>= 1)
    n ^= g;
  return n;
}