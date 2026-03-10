/**
 * Author: caterpillow
 * Date: 2025-10-24
 * Description: i asked chatgpt for a struct that gave better precision floats
 * Status: literally no clue
 */
#pragma once

using ldb = long double;
struct qdlong {
    ldb hi;   
    ldb lo;   

    static inline void twoSum(ldb a, ldb b, ldb &s, ldb &err) {
        s = a + b;
        ldb bv = s - a;
        err = (a - (s - bv)) + (b - bv);
    }

    static inline void quickTwoSum(ldb a, ldb b, ldb &s, ldb &err) {
        s = a + b;
        err = b - (s - a);
    }

    static inline void twoProd(ldb a, ldb b, ldb &p, ldb &err) {
        p = a * b;
        err = std::fma(a, b, -p);
    }

    qdlong() noexcept : hi(0.0L), lo(0.0L) {}
    qdlong(ldb x) noexcept : hi(x), lo(0.0L) {}
    qdlong(ldb hi_, ldb lo_) noexcept : hi(hi_), lo(lo_) {}

    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    qdlong(T v) noexcept : hi(static_cast<ldb>(v)), lo(0.0L) {}

    inline void normalize() noexcept {
        ldb s, e;
        quickTwoSum(hi, lo, s, e);
        hi = s;
        lo = e;
    }

    inline ldb to_long_double() const noexcept {
        return hi + lo;
    }

    friend qdlong operator+(qdlong a, const qdlong& b) noexcept {
        ldb s, e;
        twoSum(a.hi, b.hi, s, e);
        e += a.lo;
        e += b.lo;
        ldb hi_out, lo_out;
        quickTwoSum(s, e, hi_out, lo_out);
        return qdlong(hi_out, lo_out);
    }

    friend qdlong operator-(qdlong a, const qdlong& b) noexcept {
        return a + qdlong(-b.hi, -b.lo);
    }

    friend qdlong operator*(const qdlong &a, const qdlong &b) noexcept {
        ldb p1, p1err;
        twoProd(a.hi, b.hi, p1, p1err);
        ldb cross = a.hi * b.lo + a.lo * b.hi;
        ldb s, e;
        twoSum(p1err, cross, s, e);
        e += a.lo * b.lo;
        ldb hi_out, lo_out;
        twoSum(p1, s, hi_out, lo_out);
        lo_out += e;
        qdlong res(hi_out, lo_out);
        res.normalize();
        return res;
    }

    friend qdlong operator/(const qdlong &a, const qdlong &b) noexcept {
        ldb q1 = a.hi / b.hi;
        qdlong prod = b * qdlong(q1);
        qdlong r = a - prod;
        ldb q2 = (r.hi) / b.hi; // one correction term (good accuracy)
        qdlong result = qdlong(q1 + q2);
        // optional extra correction (commented out; enable if you want extra accuracy):
        // qdlong prod2 = b * result;
        // qdlong r2 = a - prod2;
        // result.hi += r2.hi / b.hi;
        result.normalize();
        return result;
    }

    friend bool operator==(const qdlong &a, const qdlong &b) noexcept {
        return a.hi == b.hi && a.lo == b.lo;
    }
    friend bool operator<(const qdlong &a, const qdlong &b) noexcept {
        if (a.hi < b.hi) return true;
        if (a.hi > b.hi) return false;
        return a.lo < b.lo;
    }

    friend qdlong abs(const qdlong &x) noexcept { return (x.hi < 0 ? (0 - x) : x); }
    friend qdlong sqrt(const qdlong &x) noexcept {
        ldb approx = std::sqrt(x.to_long_double());
        if (approx == 0.0L) return qdlong(0.0L);
        qdlong y(approx);
        for (int i = 0; i < 3; ++i) {
            y = (y + x / y) * qdlong(0.5L);
        }
        return y;
    }
};