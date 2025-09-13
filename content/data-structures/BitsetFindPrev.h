/**
 * Author: caterpillow
 * Date: 2025-09-12
 * License: CC0
 * Source: nyaan
 * Description: Implements find\_prev for bitsets.
 * Status: just works
 */
#pragma once

template <size_t Nb>
struct Bitset : bitset<Nb> {
  template <typename... Args>
  Bitset(Args... args) : bitset<Nb>(args...) {}

  static constexpr int N = Nb;
  static constexpr int array_size = (Nb + 63) / 64;

  union raw_cast {
    array<uint64_t, array_size> a;
    Bitset b;
  };

  int _Find_prev(size_t i) const {
    if (i == 0) return -1;
    if ((*this)[--i] == true) return i;
    size_t M = i / 64;
    const auto& a = ((raw_cast*)(this))->a;
    uint64_t buf = a[M] & ((1ull << (i & 63)) - 1);
    if (buf != 0) return M * 64 + 63 - __builtin_clzll(buf);
    while (M--) {
      if (a[M] != 0) return M * 64 + 63 - __builtin_clzll(a[M]);
    }
    return -1;
  }

  inline int _Find_last() const { return _Find_prev(N); }
};