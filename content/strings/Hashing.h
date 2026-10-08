/**
 * Author: Simon Lindholm
 * Date: 2015-03-15
 * License: CC0
 * Source: own work
 * Description: Substring hashes of a string or vector of values $\ge 0$.
 *  Skip the stuff that starts with r if you don't care about reverse hashes etc.
 * Usage:
 *  HashInterval h(s); // s is an lvalue
 *  h.hash_interval(l, r); // hash of s[l, r)
 *  h.rhash_interval(l, r); // hash of reversed s[l, r)
 *  // s[l, r) is a palindrome iff the two are equal
 *  get_hashes(s, len); // hashes of all windows of length len
 *  hash_string(s);
 * Status: stress-tested
 */
#pragma once

#include "BigMod.h"

struct HashInterval {
    vt<H> ha, pw, rha;
    template<class T>
    HashInterval(T& str) : ha(size(str) + 1), pw(ha), rha(ha) {
        pw[0] = 1;
        F0R (i, size(str)) {
            ha[i + 1] = ha[i] * C + str[i] + 1;
            pw[i + 1] = pw[i] * C;
        }
        ROF (i, 0, size(str)) rha[i] = rha[i + 1] * C + str[i] + 1;
    } // <hash>
    H hash_interval(int l, int r) { // hash [l, r)
        return ha[r] - ha[l] * pw[r - l];
    }
    H rhash_interval(int l, int r) { // hash of reversed [l, r)
        return rha[l] - rha[r] * pw[r - l];
    }
};

// get all hashes of length <len>
template<class T>
vt<H> get_hashes(T& str, int length) {
    if (size(str) < length) return {};
    H h = 0, pw = 1;
    F0R (i, length) h = h * C + str[i] + 1, pw = pw * C;
    vt<H> ret = {h};
    FOR (i, length, size(str)) {
        ret.pb(h = h * C + str[i] + 1
            - pw * (str[i - length] + 1));
    }
    return ret;
}

template<class T>
H hash_string(T& s) {
    H h = 0;
    for (auto c : s) h = h * C + c + 1;
    return h;
}