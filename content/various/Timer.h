/**
 * Author: caterpillow
 * Date: 2025-09-13
 * License: CC0
 * Source: nyaan 
 * Description: Timer object.
 * Status: bruh
 */
#pragma once

#include <chrono>
using namespace std;

struct Timer {
  chrono::high_resolution_clock::time_point st;

  Timer() { reset(); }
  void reset() { st = chrono::high_resolution_clock::now(); }

  long long elapsed() {
    auto ed = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(ed - st).count();
  }
  long long operator()() { return elapsed(); }
};