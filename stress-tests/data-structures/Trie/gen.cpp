// #pragma GCC optimize("O3,unroll-loops")

#include <bits/extc++.h>

using namespace std;
using namespace __gnu_pbds;

// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
// #define INTERACTIVE 

__gnu_cxx::sfmt19937 mt((uint32_t) chrono::steady_clock::now().time_since_epoch().count());

template<typename T>
class vt : public std::vector<T> {
public:
    using std::vector<T>::vector;	  
    using std::vector<T>::push_back;   

    template<class U, class = decltype(std::begin(std::declval<U>()))>
    typename std::enable_if<!std::is_same<U, std::string>::value>::type
    push_back(const U& v) {
        for (auto&& i : v) this->push_back(i);
    }

    template<typename... Args>
    void push_back(Args&&... args) {
        (this->std::vector<T>::push_back(std::forward<Args>(args)), ...);
    }

    template<class arg>
    vt<T> &operator+=(arg&& val) { this->push_back(std::forward<arg>(val)); return *this; }

    vt<T> &sort() {
        std::sort(this->begin(), this->end());
        return *this;
    }

    template<class Comp>
    vt<T> &sort(Comp comp) {
        std::sort(this->begin(), this->end(), comp);
        return *this;
    }

    int find(const T& x) {
        for (int i = 0; i < static_cast<int>(this->size()); ++i) if ((*this)[i] == x) return i;
        return this->size();
    }

    bool has(const T& x) { return find(x) != this->size(); }

    T min() const {
        if (this->empty()) {
            if constexpr (std::is_arithmetic<T>::value) return std::numeric_limits<T>::max();
            else return T{};
        }
        return *std::min_element(this->begin(), this->end());
    }

    T max() const {
        if (this->empty()) {
            if constexpr (std::is_arithmetic<T>::value) return std::numeric_limits<T>::lowest();
            else return T{};
        }
        return *std::max_element(this->begin(), this->end());
    }
    template<class U = T>
    U sum(U init = U{}) const { U s = init; for (const auto& e : *this) s += static_cast<U>(e); return s; }
    vt<T> &iota(T s = T{}) { for (int i = 0; i < this->size(); i++) (*this)[i] = s++; return *this; }
    vt<T> &rem_dupe() { sort(); this->erase(unique(this->begin(), this->end()), this->end()); return *this; }
    vt<T> &shuffle() { std::shuffle(this->begin(), this->end(), mt); return *this; }
    int lower_bound(const T &x) const { return std::lower_bound(this->begin(), this->end(), x) - this->begin(); }
    int upper_bound(const T &x) const { return std::upper_bound(this->begin(), this->end(), x) - this->begin(); }
};

template <class T>
using vvt = vt<vt<T>>;
template <class T>
using vvvt = vt<vvt<T>>;
template <class T>
using vvvvt = vector<vvvt<T>>;
template <class T>
using vvvvvt = vector<vvvvt<T>>;

#define vv(type, name, h, ...) \
    vvt<type> name(h, vt<type>(__VA_ARGS__))
#define vvv(type, name, h, w, ...)   \
    vvvt<type> name( \
        h, vvt<type>(w, vt<type>(__VA_ARGS__)))
#define vvvv(type, name, a, b, c, ...)	   \
    vvvvt<type> name( \
        a, vvvt<type>(	   \
            b, vvt<type>(c, vt<type>(__VA_ARGS__))))

using str = string;
using ll = long long;
using ull = unsigned long long;
using lll = __int128_t;
using ulll = __uint128_t;
using db = double;
using ldb = long double;
using pi = pair<int, int>;
using a2i = array<int, 2>;
using a3i = array<int, 3>;
using a4i = array<int, 4>;
using a2l = array<ll, 2>;
using a3l = array<ll, 3>;
using a4l = array<ll, 4>;
using pl = pair<ll, ll>;
using pd = pair<db, db>;
using vi = vt<int>;
using vl = vt<ll>;
using vb = vt<bool>;
using vdb = vt<db>;
using vvi = vt<vt<int>>;
using vvl = vt<vt<ll>>;
using vvb = vt<vt<bool>>;
using vs = vt<str>;
using vpi = vt<pi>;
using vpl = vt<pl>;

const ll mod = 998244353;
/*
const ll mod = 1e9 + 7;
*/
const int precision = 24;

const int RANDOM = chrono::high_resolution_clock::now().time_since_epoch().count();

struct chash { // large odd number for C
    const uint64_t C = ll(4e18 * acos(0)) | 71;
    ll operator()(ll x) const { return __builtin_bswap64((x ^ RANDOM) * C); }
};

#define gptable gp_hash_table
#define priority_queue std::priority_queue
template<class T>
using rpriority_queue = priority_queue<T, vt<T>, greater<>>;
template<class K, class V, class hash> gptable<K, V, hash> make_gptable() {
    return gptable<K, V, hash>({}, {}, {}, {}, {1 << 16});
}

#define PP_GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8, NAME, ...) NAME
#define PP_DISPATCH9(name, ...) \
  PP_GET_MACRO(_0 __VA_OPT__(,) __VA_ARGS__, name##8, name##7, name##6, name##5, name##4, name##3, name##2, name##1, name##0)

#define f first
#define s second
#define pb push_back
#define eb emplace_back
#define bg(x) begin(x)
#define en(x) end(x) 
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define size0() size()
#define size1(x) (static_cast<int>((x).size()))
#define size(...) PP_DISPATCH9(size, __VA_ARGS__)(__VA_ARGS__)
#define sq(x) ((x) * (x))
#define rsz resize
#define ins insert
#define ft front()
#define bk back()
#ifndef LOCAL
#define endl '\n'
#endif

#define lwb lower_bound
#define upb upper_bound

#define cmp(exp) [&] (const auto& x, const auto& y) { return exp; }

#define FOR1(a) for (int _ = 0; _ < (a); _++)
#define FOR2(i, b) for (int i = 0; i < (b); i++)
#define FOR3(i, a, b) for (int i = (a); i < (b); i++)
#define ROF2(i, b) for (int i = (b) - 1; i >= 0; i--)
#define ROF3(i, a, b) for (int i = (b) - 1; i >= (a); i--)
#define F0R(i, b) for (int i = 0; i < (b); i++)

#define each2(a, x) for (auto& a : x)
#define each3(a, b, x) for (auto& [a, b] : x)
#define each4(a, b, c, x) for (auto& [a, b, c] : x)
#define each5(a, b, c, d, x) for (auto& [a, b, c, d] : x)
#define reach2(a, x) for (auto it = rbegin(x); it != rend(x); it++) if (auto& a = *it; 1)
#define reach3(a, b, x) for (auto it = rbegin(x); it != rend(x); it++) if (auto& [a, b] = *it; 1)
#define reach4(a, b, c, x) for (auto it = rbegin(x); it != rend(x); it++) if (auto& [a, b, c] = *it; 1)
#define reach5(a, b, c, d, x) for (auto it = rbegin(x); it != rend(x); it++) if (auto& [a, b, c, d] = *it; 1)

#define enum3(i, a, x) if (int i = 0; 1) for (auto it = begin(x); it != end(x); it++, i++) if (auto& a = *it; 1)
#define enum4(i, a, b, x) if (int i = 0; 1) for (auto it = begin(x); it != end(x); it++, i++) if (auto& [a, b] = *it; 1)
#define enum5(i, a, b, c, x) if (int i = 0; 1) for (auto it = begin(x); it != end(x); it++, i++) if (auto& [a, b, c] = *it; 1)
#define renum3(i, a, x) if (int i = size(x) - 1; 1) for (auto it = rbegin(x); it != rend(x); it++, i--) if (auto& a = *it; 1)
#define renum4(i, a, b, x) if (int i = size(x) - 1; 1) for (auto it = rbegin(x); it != rend(x); it++, i--) if (auto& [a, b] = *it; 1)
#define renum5(i, a, b, c, x) if (int i = size(x) - 1; 1) for (auto it = rbegin(x); it != rend(x); it++, i--) if (auto& [a, b, c] = *it; 1)

#define FOR(...) PP_DISPATCH9(FOR, __VA_ARGS__)(__VA_ARGS__)
#define ROF(...) PP_DISPATCH9(ROF, __VA_ARGS__)(__VA_ARGS__)
#define each(...) PP_DISPATCH9(each, __VA_ARGS__)(__VA_ARGS__)
#define reach(...) PP_DISPATCH9(reach, __VA_ARGS__)(__VA_ARGS__)
#define enum(...) PP_DISPATCH9(enum, __VA_ARGS__)(__VA_ARGS__)
#define renum(...) PP_DISPATCH9(renum, __VA_ARGS__)(__VA_ARGS__)

#define lam1(x) [&] () { return x; }
#define lam2(x, y) [&] (x) { return y;}
#define lam3(x, y, z) [&] (x, y) { return z; }
#define lam4(w, x, y, z) [&] (w, x, y) { return z; }

#define lam(...) PP_DISPATCH9(lam, __VA_ARGS__)(__VA_ARGS__)

template<class Fun> class y_combinator_result {
    Fun fun_;
public:
    template<class T> explicit y_combinator_result(T &&fun): fun_(std::forward<T>(fun)) {}
    template<class ...Args> decltype(auto) operator()(Args &&...args) { return fun_(std::ref(*this), std::forward<Args>(args)...); }
};
template<class Fun> decltype(auto) ycomb(Fun &&fun) { return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun)); }

ll mpow(ll x, ll y = mod - 2) {
    ll res = 1;
    for (; y; x = (x * x) % mod, y >>= 1) if (y & 1) res = (res * x) % mod;
    return res;
}
ll mpow(ll x, ll y, ll mod) {
    ll res = 1;
    for (; y; x = (x * x) % mod, y >>= 1) if (y & 1) res = (res * x) % mod;
    return res;
}
ll gen(ll l, ll r) { return uniform_int_distribution<ll>(l, r)(mt); }

int rs, cs;
const int dr[5] {0, -1, 0, 1, 0}, dc[5] {1, 0, -1, 0, 0};
bool valid(int r, int c) { return r >= 0 && c >= 0 && r < rs && c < cs; }
vpi get_adj(int r, int c) {
    vpi ret(4);
    FOR (i, 4) ret[i] = {r + dr[i], c + dc[i]};
    return ret;
}
vpi valid_adj(int r, int c) {
    vpi ret;
    FOR (i, 4) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (valid(nr, nc)) ret.eb(nr, nc);
    }
    return ret;
}

constexpr int pct(int x) { return __builtin_popcount(x); }
constexpr int pctl(ll x) { return __builtin_popcountll(x); }
// 2 ^ bits(n) is the largest power of 2 <= n
constexpr int bits(int x) { return x ? 31 - __builtin_clz(x) : 0; }
constexpr int bits(ll x) { return x ? 63 - __builtin_clzll(x) : 0; }
constexpr int p2(int x) { return 1 << x; }
constexpr ll p2l(int x) { return 1ll << x; }
constexpr int msk2(int x) { return p2(x) - 1; }
constexpr ll msk2l(int x) { return p2l(x) - 1; }

ll cdiv(ll a, ll b) { return a / b + ((a ^ b) > 0 && a % b); }
ll fdiv(ll a, ll b) { return a / b - ((a ^ b) < 0 && a % b); }

// -------------------- Debug (cerr) helpers --------------------
// Pairs
template<class T, class U>
ostream& operator<<(ostream& os, const pair<T,U>& p) { return os << '{' << p.f << ", " << p.s << '}'; }

// Tuples
template<typename... Args>
ostream& operator<<(ostream& os, const tuple<Args...>& t) {
    os << '{';
    string sep;
    apply([&](const Args&... xs){ ((os << exchange(sep, ", ") << xs), ...); }, t);
    return os << '}';
}

// Iterable detection (exclude string)
template<class T, class = void>
struct is_iterable : false_type {};
template<class T>
struct is_iterable<T, void_t<decltype(begin(declval<T>())), decltype(end(declval<T>()))>>
    : bool_constant<!is_same_v<T, string>> {};

// Ranges
template<class T>
enable_if_t<is_iterable<T>::value, ostream&>
operator<<(ostream& os, const T& v) {
    os << '{';
    string sep;
    for (auto&& x : v) os << exchange(sep, ", ") << x;
    return os << '}';
}

// lll to/from streams (for cerr and LOCAL-style manual reads)
static inline ostream& operator<<(ostream& os, lll x) {
    if (x == 0) return os << '0';
    if (x < 0) os << '-', x = -x;
    string s; s.reserve(40);
    while (x) { int d = int(x % 10); s.push_back(char('0' + d)); x /= 10; }
    reverse(all(s));
    return os << s;
}
static inline istream& operator>>(istream& is, lll& x) {
    string t; is >> t;
    int i = 0, sign = 1; if (i < size(t) && t[i] == '-') sign = -1, ++i;
    lll v = 0; for (; i < size(t); ++i) v = v * 10 + (t[i] - '0');
    x = v * sign; return is;
}

// -------------------- Fast buffered IO core --------------------
namespace IO {
    static constexpr int BSZ = 1 << 15;

#ifndef INTERACTIVE
    static char ibuf[BSZ];
    static int  ipos = 0, ilen = 0;
#endif

#if defined(_WIN32)
#define IO_GETC() getchar()
#else
#define IO_GETC() getchar_unlocked()
#endif

    static inline char next_char_raw(void) {
#ifndef INTERACTIVE
        if (__builtin_expect(ipos >= ilen, 0)) {
            ipos = 0;
            ilen = (int)fread(ibuf, 1, BSZ, stdin);
            if (!ilen) return -1;
        }
        return ibuf[ipos++];
#else
        char c = IO_GETC();
        return (c == EOF) ? -1 : c;
#endif
    }

    static inline char skip_ws_and_get() {
        char c;
        while ((c = next_char_raw()) != EOF && isspace((unsigned char)c)) {}
        return c;
    }

    template<class T, enable_if_t<is_integral_v<T> && !is_same_v<T,bool> && !is_same_v<T,lll> && !is_same_v<T,ulll>, int> = 0>
    inline void read_integer(T &x) {
        char c = skip_ws_and_get();
        int sign = 1;
        if (c == '-') sign = -1, c = next_char_raw();
        T v = 0;
        for (; c != EOF && isdigit((unsigned char)c); c = next_char_raw()) v = v * 10 + (c - '0');
        x = (T)(v * sign);
    }
    inline void read_char(char &c) { c = skip_ws_and_get(); }
    inline void read_string(string &s) {
        s.clear();
        char c = skip_ws_and_get();
        for (; c != EOF && !isspace((unsigned char)c); c = next_char_raw()) s.push_back(c);
    }
    inline void read_long_double(long double &x) { string t; read_string(t); x = stold(t); }
    inline void read_double(double &x)		   { long double t; read_long_double(t); x = (double)t; }
    inline void read_float(float &x)			 { long double t; read_long_double(t); x = (float)t; }
    inline void read_lll(lll &x) {
        string t; read_string(t);
        int i = 0, sign = 1; if (i < (int)t.size() && t[i]=='-') sign = -1, ++i;
        lll v = 0; for (; i < (int)t.size(); ++i) v = v*10 + (t[i]-'0');
        x = v * sign;
    }

    // Output buffer
    static char obuf[BSZ];
    static int  opos = 0;

    static inline void flush_out() {
        if (opos) { fwrite(obuf, 1, opos, stdout); opos = 0; fflush(stdout); }
    }
    static inline void put_char(char c) {
        if (opos == BSZ) flush_out();
        obuf[opos++] = c;
#ifdef INTERACTIVE
        if (c == '\n') flush_out(); // auto-flush for interactives
#endif
    }

    template<class T, enable_if_t<is_integral_v<T> && !is_same_v<T,bool> && !is_same_v<T,lll> && !is_same_v<T,ulll>, int> = 0>
    inline void write_integer(T x) {
        if (x == 0) { put_char('0'); return; }
        if (x < 0) put_char('-'), x = -x;
        char buf[32]; int n = 0;
        while (x) { buf[n++] = char('0' + (x % 10)); x /= 10; }
        while (n--) put_char(buf[n]);
    }
    inline void write_lll(lll x) {
        if (x == 0) { put_char('0'); return; }
        if (x < 0) put_char('-'), x = -x;
        char buf[64]; int n = 0;
        while (x) { int d = int(x % 10); buf[n++] = char('0' + d); x /= 10; }
        while (n--) put_char(buf[n]);
    }
    inline void write_cstr(const char* s) { while (*s) put_char(*s++); }
    inline void write_str(const string& s) { for (char c : s) put_char(c); }

    inline void init() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        atexit([]{ flush_out(); });
    }
    inline void flush() { flush_out(); }
} // namespace IO

// -------------------- READ (unqualified for ADL) --------------------
inline void read(char &x) { IO::read_char(x); }
inline void read(string &x) { IO::read_string(x); }
inline void read(bool &x) { char ch; read(ch); x = (ch == '1'); }

template<class T, enable_if_t<is_integral_v<T> && !is_same_v<T,bool> && !is_same_v<T,lll> && !is_same_v<T,ulll>, int> = 0>
inline void read(T &x) { IO::read_integer(x); }
inline void read(lll &x) { IO::read_lll(x); }
inline void read(long double &x) { IO::read_long_double(x); }
inline void read(double &x) { IO::read_double(x); }
inline void read(float &x) { IO::read_float(x); }

template<class T, class U>
inline void read(pair<T,U> &p) { read(p.f); read(p.s); }

template<typename... Args>
inline void read(tuple<Args...> &t) { apply([&](auto&... xs){ (read(xs), ...); }, t); }

template<class T>
enable_if_t< is_iterable<T>::value
          && (!std::is_same_v<typename T::value_type, bool>) >
inline read(T& v) { for (auto &e : v) read(e); }

template<class C>
inline auto read(C& v)
    -> enable_if_t< is_same_v<typename C::value_type, bool> > {
    for (int i = 0; i < size(v); i++) {
        char c; read(c);
        v[i] = (c == '1');
    }
}

template<class T, class... Ts>
inline void read(T &t, Ts&... ts) { read(t); if constexpr (sizeof...(ts)) read(ts...); }

// -------------------- _print (unqualified for ADL) --------------------
inline void _print(char c) { IO::put_char(c); }
inline void _print(const char* s) { IO::write_cstr(s); }
inline void _print(const string& s) { IO::write_str(s); }
inline void _print(bool x) { IO::write_cstr(x ? "1" : "0"); }

template<class T, enable_if_t<is_integral_v<T> && !is_same_v<T,bool> && !is_same_v<T,lll> && !is_same_v<T,ulll>, int> = 0>
inline void _print(T x) { IO::write_integer(x); }
inline void _print(lll x) { IO::write_lll(x); }

inline void _print(float x)  { ostringstream o; o.setf(ios::fixed); o << setprecision(6)  << x; IO::write_str(o.str()); }
inline void _print(double x) { ostringstream o; o.setf(ios::fixed); o << setprecision(15) << x; IO::write_str(o.str()); }
inline void _print(long double x) { ostringstream o; o.setf(ios::fixed); o << setprecision(18) << x; IO::write_str(o.str()); }

template<class T, class U>
inline void _print(const pair<T, U>& p) { _print(p.f); _print(' '); _print(p.s); }

template<typename... Args>
inline void _print(const tuple<Args...>& t) {
    string sep;
    apply([&](const auto&... xs){ ((_print(sep.c_str()), _print(xs), sep = " "), ...); }, t);
}

template<class T>
enable_if_t<is_iterable<T>::value>
inline _print(const T& v) {
    string sep;
    for (auto&& x : v) { _print(sep.c_str()); _print(x); sep = " "; }
}

// -------------------- print / println / flushing --------------------
inline void print() {} // base

template<class T>
inline void print(const T& t) { _print(t); _print(' '); }

template<class T, class... Ts>
inline void print(const T& t, const Ts&... ts) { _print(t); _print(' '); print(ts...); }

inline void println() { _print('\n'); }
template<class T>
inline void println(const T& t) { _print(t); _print('\n'); }
template<class T, class... Ts>
inline void println(const T& t, const Ts&... ts) { _print(t); _print(' '); println(ts...); }

// -------------------- file helpers --------------------
inline void setfileaio(const string& name) {
#ifndef LOCAL
    if (name.empty()) return;
    freopen((name + "in.txt").c_str(), "r", stdin);
    freopen((name + "out.txt").c_str(), "w", stdout);
#endif
}
inline void setfile(const string& pfx) {
#ifndef LOCAL
    if (pfx.empty()) return;
    freopen((pfx + ".in").c_str(), "r", stdin);
    freopen((pfx + ".out").c_str(), "w", stdout);
#endif
}

// ================== END IO TEMPLATE ==================

template <class T>
inline T _r() { T x; read(x); return x; }

#define __IO_ARG_N(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N
#define __IO_RSEQ_N() 10,9,8,7,6,5,4,3,2,1,0
#define __IO_NARG_I_(...) __IO_ARG_N(__VA_ARGS__)
#define __IO_NARG__(...)  __IO_NARG_I_(, ##__VA_ARGS__, __IO_RSEQ_N())

#define __IO_CAT_(a,b) a##b
#define __IO_CAT(a,b)  __IO_CAT_(a,b)
#define __IO_TDISPATCH(name, T, ...) __IO_CAT(name, __IO_NARG__(__VA_ARGS__))(T, ##__VA_ARGS__)

#define R0(T)
#define R1(T,a)						  T a = _r<T>()
#define R2(T,a,b)						T a = _r<T>(); T b = _r<T>()
#define R3(T,a,b,c)					  T a = _r<T>(); T b = _r<T>(); T c = _r<T>()
#define R4(T,a,b,c,d)					T a = _r<T>(); T b = _r<T>(); T c = _r<T>(); T d = _r<T>()
#define R5(T,a,b,c,d,e)				  T a = _r<T>(); T b = _r<T>(); T c = _r<T>(); T d = _r<T>(); T e = _r<T>()
#define R6(T,a,b,c,d,e,f)				T a = _r<T>(); T b = _r<T>(); T c = _r<T>(); T d = _r<T>(); T e = _r<T>(); T f = _r<T>()
#define R7(T,a,b,c,d,e,f,g)			  T a = _r<T>(); T b = _r<T>(); T c = _r<T>(); T d = _r<T>(); T e = _r<T>(); T f = _r<T>(); T g = _r<T>()
#define R8(T,a,b,c,d,e,f,g,h)			T a = _r<T>(); T b = _r<T>(); T c = _r<T>(); T d = _r<T>(); T e = _r<T>(); T f = _r<T>(); T g = _r<T>(); T h = _r<T>()
#define R9(T,a,b,c,d,e,f,g,h,i)		  T a = _r<T>(); T b = _r<T>(); T c = _r<T>(); T d = _r<T>(); T e = _r<T>(); T f = _r<T>(); T g = _r<T>(); T h = _r<T>(); T i = _r<T>()
#define R10(T,a,b,c,d,e,f,g,h,i,j)	   T a = _r<T>(); T b = _r<T>(); T c = _r<T>(); T d = _r<T>(); T e = _r<T>(); T f = _r<T>(); T g = _r<T>(); T h = _r<T>(); T i = _r<T>(); T j = _r<T>()

#define R(T, ...) __IO_TDISPATCH(R, T, __VA_ARGS__)

#define RZ0(T)
#define RZ1(T,a)						 T a = _r<T>() - (T)1
#define RZ2(T,a,b)					   T a = _r<T>() - (T)1; T b = _r<T>() - (T)1
#define RZ3(T,a,b,c)					 T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1
#define RZ4(T,a,b,c,d)				   T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1; T d = _r<T>() - (T)1
#define RZ5(T,a,b,c,d,e)				 T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1; T d = _r<T>() - (T)1; T e = _r<T>() - (T)1
#define RZ6(T,a,b,c,d,e,f)			   T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1; T d = _r<T>() - (T)1; T e = _r<T>() - (T)1; T f = _r<T>() - (T)1
#define RZ7(T,a,b,c,d,e,f,g)			 T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1; T d = _r<T>() - (T)1; T e = _r<T>() - (T)1; T f = _r<T>() - (T)1; T g = _r<T>() - (T)1
#define RZ8(T,a,b,c,d,e,f,g,h)		   T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1; T d = _r<T>() - (T)1; T e = _r<T>() - (T)1; T f = _r<T>() - (T)1; T g = _r<T>() - (T)1; T h = _r<T>() - (T)1
#define RZ9(T,a,b,c,d,e,f,g,h,i)		 T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1; T d = _r<T>() - (T)1; T e = _r<T>() - (T)1; T f = _r<T>() - (T)1; T g = _r<T>() - (T)1; T h = _r<T>() - (T)1; T i = _r<T>() - (T)1
#define RZ10(T,a,b,c,d,e,f,g,h,i,j)	  T a = _r<T>() - (T)1; T b = _r<T>() - (T)1; T c = _r<T>() - (T)1; T d = _r<T>() - (T)1; T e = _r<T>() - (T)1; T f = _r<T>() - (T)1; T g = _r<T>() - (T)1; T h = _r<T>() - (T)1; T i = _r<T>() - (T)1; T j = _r<T>() - (T)1

#define RZ(T, ...) __IO_TDISPATCH(RZ, T, __VA_ARGS__)

#define ri(...)   R(int, __VA_ARGS__)
#define rid(...)  RZ(int, __VA_ARGS__)
#define rl(...)   R(ll,  __VA_ARGS__)
#define rd(...)   R(db,  __VA_ARGS__)
#define rc(...)   R(char,__VA_ARGS__)
#define rs(...)   R(str, __VA_ARGS__)
#define rlll(...)  R(lll, __VA_ARGS__)

#define rvi(a, n) vi a(n); FOR (i, n) a[i] = _r<int>()
#define rvid(a, n) vi a(n); FOR (i, n) a[i] = _r<int>() - 1
#define rvl(a, n) vl a(n); FOR (i, n) a[i] = _r<ll>()
#define rvb(a, n) vb a(n); FOR (i, n) { a[i] = (_r<char>() == '1'); }

const int inf = 1e9;
const ll INF = 1e18;
const db eps = 1e-9;

template<class T> bool chmin(T& a, const T& b) { return b < a ? a = b, 1 : 0; }
template<class T> bool chmax(T& a, const T& b) { return a < b ? a = b, 1 : 0; }

int YES() { println("YES"); return 0; }
int NO() { println("NO"); return 0; }
int Yes() { println("Yes"); return 0; }
int No() { println("No"); return 0; }
#define ret(x) { x; return; }
#define exit(x) { x; exit(0); }

#ifdef LOCAL
template <typename T, typename... V>
void printer(string pfx, const char *names, T&& head, V&& ...tail) {
    int i = 0, brackets = 0;
    while (names[i] && (names[i] != ',' || brackets)) {
        if (names[i] == '(' || names[i] == '{') brackets++;
        if (names[i] == ')' || names[i] == '}') brackets--;
        i++;
    }
    constexpr bool is_str = is_same_v<decay_t<T>, const char*>;
    if (is_str) cerr << " " << head;
    else cerr << pfx, cerr.write(names, i) << " = " << head; 
    if constexpr (sizeof...(tail)) printer(is_str ? "" : ",", names + i + 1, tail...);
    else cerr << endl;
}

#define dbg(...) printer(to_string(__LINE__) + ": ", #__VA_ARGS__, __VA_ARGS__)
#else
#define dbg(x...)
#define cerr if (0) std::cerr
#endif

/*



*/

signed main() {
    IO::init();
    
    int q = gen(1, 100);
    println(q);
    set<int> a;
    int mx = (1 << gen(2, 30)) - 1;
    FOR (q) {
        int t = gen(0, 1 + (!!size(a)) * 4);
        int x = gen(0, mx);
        println(t, x);
        if (t == 0) a.insert(x);
        if (t == 1) a.erase(x);
        if (t == 4) println(gen(0, mx));
        if (t == 5) println(gen(0, mx));
    }
}