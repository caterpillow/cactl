/**
 * Author: caterpillow
 * Date: 2025-09-23
 * Source: me
 * Description: example beam search solution
 */
#pragma once

#pragma GCC optimize("Ofast,omit-frame-pointer,inline,unroll-all-loops")

#include <bits/extc++.h>
#include <sys/time.h>

#pragma GCC target("avx2")

using namespace std;
using ll = long long;

#define vt vector
#define f first
#define s second
#define pb push_back
#define all(x) begin(x), end(x)
#define size(x) ((int) (x).size())
#define FOR(i, a, b) for (int i = (a); i < (b); i++)
#define ROF(i, a, b) for (int i = (b) - 1; i >= (a); i--)
#define F0R(i, b) FOR (i, 0, b)
const int inf = 1e9;
const ll INF = 1e18;
using vi = vt<int>;
using db = long double;

__gnu_cxx::sfmt19937 mt(random_device{}());

int gen(int l, int r) {
    return uniform_int_distribution<int>(l, r)(mt);
}

double get_time() {timeval tv; gettimeofday(&tv, NULL); return tv.tv_sec + tv.tv_usec * 1e-6;}
double start_time = get_time();
double elapsed() {return get_time() - start_time;}

const db TIME_LIMIT = 5;

// data

int n, k;
vector<int> a;
db ik;

// end data

using T = db;

struct State {
    T value{};
    vt<vi> buckets;
    vt<db> sums;

    void print() {
        F0R (i, k) {
            for (int x : buckets[i]) cout << x << ' ';
            cout << '\n';
        }
        cerr << "value: " << fixed << setprecision(24) << value << '\n';
    }
};

class Transition {
public:
    T new_val{};
    virtual ~Transition() = default;
    virtual State accept() = 0;
    bool operator<(const Transition& oth) const { return new_val < oth.new_val; }
};

class Swap : public Transition {
    State& cur;
    int i{}, j{}, el{};
public:
    explicit Swap(State& cur_) : cur(cur_) {
        do { i = gen(0, k - 1); } while (size(cur.buckets[i]) <= 1);

        j = gen(0, k - 2);
        if (j >= i) ++j;

        el = gen(0, size(cur.buckets[i]) - 1);
        int x = cur.buckets[i][el];

        new_val = cur.value
            / powl(cur.sums[i], ik) / powl(cur.sums[j], ik)
            * powl(cur.sums[i] - x, ik) * powl(cur.sums[j] + x, ik);
    }

    State accept() override {
        State nxt = cur;                 
        int &x = nxt.buckets[i][el];
        nxt.buckets[j].pb(x);
        nxt.sums[i] -= x;
        nxt.sums[j] += x;
        swap(x, nxt.buckets[i].back());
        nxt.buckets[i].pop_back();
        nxt.value = new_val;
        return nxt;
    }
};

class Move : public Transition {
    State& cur;
    int i{}, j{}, xi{}, yi{};
public:
    explicit Move(State& cur_) : cur(cur_) {
        i = gen(0, k - 1);
        j = gen(0, k - 2);
        if (j >= i) ++j;

        xi = gen(0, size(cur.buckets[i]) - 1);
        yi = gen(0, size(cur.buckets[j]) - 1);

        int x = cur.buckets[i][xi];
        int y = cur.buckets[j][yi];

        new_val = cur.value
                / powl(cur.sums[i], ik) / powl(cur.sums[j], ik)
                * powl(cur.sums[i] - x + y, ik) * powl(cur.sums[j] - y + x, ik);
    }

    State accept() override {
        State nxt = cur;
        int &x = nxt.buckets[i][xi];
        int &y = nxt.buckets[j][yi];
        nxt.sums[j] += x - y;
        nxt.sums[i] += y - x;
        swap(x, y);
        nxt.value = new_val;
        return nxt;
    }
};

db calc_score(State& state) {
    db val = 1;
    F0R (i, k) val *= powl(state.sums[i], ik);
    return val;
}

signed main() {
    cin.tie(0)->sync_with_stdio(0);

    cin >> n >> k;
    a.resize(n);
    for (int &x : a) cin >> x;

    State cur;
    cur.buckets.resize(k);
    cur.sums.assign(k, 0);
    F0R (i, n) {
        int j = int(min_element(all(cur.sums)) - cur.sums.begin());
        cur.sums[j] += a[i];
        cur.buckets[j].push_back(a[i]);
    }
    ik = 1.0l / k;
    cur.value = calc_score(cur);

    const int beam_width = 100;
    const int neighbours = 100;

    vector<State> states{cur};
    State best = cur;

    int its = 0;
    while (elapsed() < TIME_LIMIT) {
        its++;
        vector<unique_ptr<Transition>> transitions;
        transitions.reserve(size(states) * neighbours * 2);

        for (auto &st : states) {
            if (st.value > best.value) best = st;
            F0R (_, neighbours) {
                if (k > 1) transitions.emplace_back(make_unique<Swap>(st));
                transitions.emplace_back(make_unique<Move>(st));
            }
        }

        if (transitions.empty()) break;

        sort(all(transitions), [](const auto& a, const auto& b){ return a->new_val > b->new_val; });

        vector<State> new_states;
        int take = min<int>(beam_width, size(transitions));
        new_states.reserve(take);
        F0R (i, take) new_states.push_back(transitions[i]->accept());

        states = std::move(new_states);
    }

    best.print();
}