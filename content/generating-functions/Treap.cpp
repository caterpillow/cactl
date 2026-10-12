// Treap code generated with comments
struct Node; struct Lazy; using ptr = struct Node *; ptr merge(ptr l, ptr r);

struct Lazy {
    bool rev;
    ll val;
    bool inc;
    ll kval;
    bool kinc;

    bool empty() const { return !rev && val == 0 && inc && kval == 0 && kinc; }

    void operator+=(const Lazy oth) {
        rev ^= oth.rev;
        if (!oth.inc) val = 0, inc = false;
        val += oth.val;
        if (!oth.kinc) kval = 0, kinc = false;
        kval += oth.kval;
    }
};

// You can implement your own monoid here for custom operations.
struct Value {
    ll sum;
    ll mx;
    ll ksum;

    static Value make(ll x, ll len = 1) {
        return {1LL * x * len, x, 0};
    }

    void upd(Lazy lazy, int sz);

    Value operator+(const Value& oth) const {
        Value res {};
        res.sum = sum + oth.sum;
        res.mx = max(mx, oth.mx);
        res.ksum = ksum + oth.ksum;
        return res;
    }
};

const Value vid = {0, numeric_limits<ll>::lowest(), 0};

const Lazy lid = {false, 0, true, 0, true};

mt19937 mt(chrono::high_resolution_clock::now().time_since_epoch().count());

struct Node {
    Value val, agg;
    Lazy lazy;
    ll key;
    int sz, pri;
    ptr l, r, par;

    Node(ll key = {}, Value val = Value::make(0)) : key(key), val(val), agg(val) {
        pri = mt();
        lazy = lid;
        sz = 1;
        l = r = nullptr;
        par = nullptr;
        this->val.ksum = agg.ksum = key;
    }

    ~Node() {
        delete l;
        delete r;
    }
};

void Value::upd(Lazy lazy, int sz) {
    if (!lazy.inc) sum = 0;
    if (!lazy.inc) mx = 0;
    sum += lazy.val * sz;
    mx += lazy.val;
    if (!lazy.kinc) ksum = 0;
    ksum += lazy.kval * 1LL * sz;
}

int sz(ptr n) { return n ? n->sz : 0; }

Value agg(ptr n) { return n ? n->agg : vid; }

void apply(ptr n, Lazy lazy) {
    if (!n || lazy.empty()) return;
    if (!lazy.kinc) n->key = {};
    n->key += lazy.kval;
    n->val.upd(lazy, 1);
    n->agg.upd(lazy, sz(n));
    if (lazy.rev) swap(n->l, n->r);
    n->lazy += lazy;
}

void push(ptr n) {
    if (!n || n->lazy.empty()) return;
    apply(n->l, n->lazy), apply(n->r, n->lazy);
    n->lazy = lid;
}

ptr pull(ptr n) {
    if (!n) return nullptr;
    n->sz = sz(n->l) + 1 + sz(n->r);
    n->agg = agg(n->l) + n->val + agg(n->r);
    if (n->l) n->l->par = n;
    if (n->r) n->r->par = n;
    return n;
}

ptr merge(ptr l, ptr r) {
    if (l) l->par = nullptr;
    if (r) r->par = nullptr;
    if (!l || !r) return l ? l : r;
    if (l->pri > r->pri)
        return push(l), l->r = merge(l->r, r), pull(l);
    else
        return push(r), r->l = merge(l, r->l), pull(r);
}

// (-inf, k) and [k, inf)
void split(ptr n, ll k, ptr &l, ptr &r, bool inclusive = false) {
    if (!n) { l = r = nullptr; return; }
    push(n);
    if (n->key < k || (inclusive && !(k < n->key))) split(n->r, k, n->r, r, inclusive), pull(l = n);
    else split(n->l, k, l, n->l, inclusive), pull(r = n);
    if (l) l->par = nullptr;
    if (r) r->par = nullptr;
}

pair<ptr, ptr> split(ptr n, ll k) { ptr l, r; split(n, k, l, r); return {l, r}; }
pair<ptr, ptr> split_le(ptr n, ll k) { ptr l, r; split(n, k, l, r, true); return {l, r}; }

// (-inf, i) and [i, inf)
void spliti(ptr n, int i, ptr &l, ptr &r) {
    if (!n) { l = r = nullptr; return; }
    push(n);
    if (i <= sz(n->l)) spliti(n->l, i, l, n->l), pull(r = n);
    else spliti(n->r, i - 1 - sz(n->l), n->r, r), pull(l = n);
    if (l) l->par = nullptr;
    if (r) r->par = nullptr;
}

pair<ptr, ptr> spliti(ptr n, int i) { ptr l, r; spliti(n, i, l, r); return {l, r}; }

ptr find(ptr n, ll k) {
    if (!n) return nullptr;
    push(n);
    if (n->key == k) return n;
    if (k <= n->key) return find(n->l, k);
    else return find(n->r, k);
}

ptr findi(ptr n, int i) {
    if (!n) return nullptr;
    push(n);
    if (sz(n->l) == i) return n;
    if (i < sz(n->l)) return findi(n->l, i);
    else return findi(n->r, i - sz(n->l) - 1);
}

// only insert single nodes
void ins(ptr& n, ptr it) {
    if (!n) { n = it; return; }
    push(n);
    if (n->pri < it->pri) split(n, it->key, it->l, it->r), n = it;
    else if (it->key <= n->key) ins(n->l, it);
    else ins(n->r, it);
    pull(n);
}

// returns pointer to deleted node
ptr del(ptr& n, ll k) {
    if (!n) return nullptr;
    push(n);
    if (n->key == k) { ptr ret = n; n = merge(n->l, n->r); ret->l = ret->r = nullptr; ret->par = nullptr; return pull(ret); }
    ptr ret = k <= n->key ? del(n->l, k) : del(n->r, k);
    pull(n);
    return ret;
}

// inserts node such that it will be at index i. only insert single nodes
void insi(ptr& n, ptr it, int i) {
    if (!n) { n = it; return; }
    push(n);
    if (n->pri < it->pri) spliti(n, i, it->l, it->r), n = it;
    else if (i <= sz(n->l)) insi(n->l, it, i);
    else insi(n->r, it, i - 1 - sz(n->l));
    pull(n);
}

// returns pointer to deleted node
ptr deli(ptr& n, int i) {
    if (!n) return nullptr;
    push(n);
    if (i == sz(n->l)) { ptr ret = n; n = merge(n->l, n->r); ret->l = ret->r = nullptr; ret->par = nullptr; return pull(ret); }
    ptr ret = i <= sz(n->l) ? deli(n->l, i) : deli(n->r, i - 1 - sz(n->l));
    pull(n);
    return ret;
}

// performs an arbitrary operation on some node
template <typename Op>
void modify(ptr n, ll k, Op op) {
    if (!n) return;
    push(n);
    if (n->key == k) op(n);
    else if (k <= n->key) modify(n->l, k, op);
    else modify(n->r, k, op);
    pull(n);
}

void clean(ptr n) {
    if (!n) return;
    clean(n->par);
    push(n);
}

int _order(ptr n, ptr from) {
    if (!n) return -1;
    int res = _order(n->par, n);
    if (from == n->r || !from) res += sz(n->l) + 1;
    return res;
}

int order(ptr n) { clean(n); return _order(n, nullptr); }

ptr root(ptr n) {
    if (!n) return nullptr;
    while (n->par) n = n->par;
    return n;
}

// finds the first iter in treap where bool(*iter < k) is false
ptr lower_bound(ptr n, ll k) {
    if (!n) return nullptr;
    push(n);
    if (n->key < k) return lower_bound(n->r, k);
    ptr rhs = lower_bound(n->l, k);
    return rhs ? rhs : n;
}

void heapify(ptr n) {
    if (!n) return;
    ptr mx = n;
    if (n->l && n->l->pri > mx->pri) mx = n->l;
    if (n->r && n->r->pri > mx->pri) mx = n->r;
    if (mx != n) swap(n->pri, mx->pri), heapify(mx);
}

ptr build(vector<ptr>& ns, int l = 0, int r = -69) {
    if (r == -69) r = (int) ns.size() - 1;
    if (l > r) return nullptr;
    int m = l + (r - l) / 2;
    ptr n = ns[m];
    push(n);
    n->par = nullptr;
    n->l = build(ns, l, m - 1);
    n->r = build(ns, m + 1, r);
    heapify(n);
    return pull(n);
}

template <typename Op>
void tour(ptr n, Op op) {
    stack<ptr> stk;
    while (n || !stk.empty()) {
        for (; n; n = n->l) push(n), stk.push(n);
        n = stk.top(); stk.pop();
        op(n);
        n = n->r;
    }
}

// you CANNOT use the normal range update for range reverses
void reversei(ptr& n, int lo, int hi) {
    auto [lm, r] = spliti(n, hi);
    auto [l, m] = spliti(lm, lo);
    Lazy upd = lid;
    upd.rev = true;
    apply(m, upd);
    n = merge(merge(l, m), r);
}

void updi(ptr n, int lo, int hi, Lazy lazy) {
    lo = max(lo, 0), hi = min(hi, sz(n));
    if (!n || lo >= hi || lazy.empty()) return;
    if (lazy.rev) throw invalid_argument("Use reverse/reversei for range reversal");
    if (lo == 0 && n->sz == hi) {
        apply(n, lazy);
        return;
    }
    push(n);
    if (lo <= sz(n->l) && sz(n->l) < hi) {
        n->val.upd(lazy, 1);
        if (!lazy.kinc) n->key = {};
        n->key += lazy.kval;
    }
    updi(n->l, lo, hi, lazy);
    updi(n->r, lo - 1 - sz(n->l), hi - 1 - sz(n->l), lazy);
    pull(n);
}

void upd(ptr& n, ll lo, ll hi, Lazy lazy) {
    auto [lm, r] = split(n, hi);
    auto [l, m] = split(lm, lo);
    apply(m, lazy);
    n = merge(l, merge(m, r));
}

Value query(ptr& n, ll lo, ll hi) {
    auto [lm, r] = split(n, hi);
    auto [l, m] = split(lm, lo);
    Value res = agg(m);
    n = merge(l, merge(m, r));
    return res;
}

Value queryi(ptr n, int lo, int hi) {
    lo = max(lo, 0), hi = min(hi, sz(n));
    if (!n || lo >= hi) return vid;
    if (lo <= 0 && sz(n) <= hi) return n->agg;
    push(n);
    return queryi(n->l, lo, hi) + (lo <= sz(n->l) && sz(n->l) < hi ? n->val : vid) + queryi(n->r, lo - 1 - sz(n->l), hi - 1 - sz(n->l));
}
