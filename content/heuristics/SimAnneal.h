/**
 * Author: caterpillow
 * Date: 2025-09-23
 * Source: me
 * Description: template for simulated annealing
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
using db = double;

__gnu_cxx::sfmt19937 mt(random_device{}());

int gen(int l, int r) {
    return uniform_int_distribution<int>(l, r)(mt);
}
db next_double() { 
    return uniform_real_distribution<db>(0, 1)(mt);
}

double get_time() {timeval tv; gettimeofday(&tv, NULL); return tv.tv_sec + tv.tv_usec * 1e-6;}
double start_time = get_time();
double elapsed() {return get_time() - start_time;}

const db TIME_LIMIT = 0.95;
db t0 = 1e9, tn = 0.1, time_passed = 1e-9;
const db maximise_score = -1; // -1 if minimise

// data

int n;
vector<int> a;

// end data

using T = int;
struct State {
    T value;
    vi a;

    // static transition: consider optimising to dynamic
    void to_neighbour() {
        int i, j;
        switch(gen(0, 4)) {
            case 0:
                i = gen(0, n - 1); 
                j = gen(0, n - 2);
                if (i == j) j++;
                swap(a[i], a[j]);
                break;
            case 1:
                a[gen(0, n - 1)] *= -1;
                break;
            case 2:
            case 3:
                i = gen(0, n);
                j = gen(0, n);
                if (i > j) swap(i, j);
                reverse(a.begin() + i, a.begin() + j);
                break;
            case 4:
                i = gen(0, n);
                j = gen(0, n);
                if (i > j) swap(i, j);
                FOR (k, i, j) a[k] *= -1;
                break;
        }
    }

    int calc_value() {
        T ans = 0, sum = 0;
        for (int x : a) sum += x, ans += abs(sum);
        return value = ans;
    }

    void print() {
        F0R (i, n) {
            if (a[i] > 0) cout << a[i] << " a\n";
            else cout << -a[i] << " f\n";
        }
        cout << value << '\n';
    }    
};

State cur, best;

signed main() {
    cin.tie(0)->sync_with_stdio(0);

    cin >> n;
    a.resize(n);
    for (int &x : a) cin >> x;
    sort(all(a)); 
    F0R (i, n) if (a[i] % 4 == 1 || a[i] % 4 == 2) a[i] *= -1;
    cur.a = a;
    cur.calc_value();
    best = cur;
    t0 = cur.value * 2;

    if (n == 1) {
        cur.print();
        return 0;
    }

    int its = 0;
    db t;
    while (true) {
        if ((its & 511) == 0) {
            time_passed = elapsed() / TIME_LIMIT;
            if (time_passed > 1.0) break;
            t = t0 * pow(tn / t0, time_passed);
        }
        its++;

        State neighbour = cur;
        neighbour.to_neighbour();
        if ((neighbour.calc_value() - cur.value) * maximise_score >= 0 
            || next_double() < exp(((neighbour.value - cur.value) * maximise_score) / t)) {
            cur = neighbour;
            if ((cur.value - best.value) * maximise_score > 0) {
                best = cur;
            }
        }
    }
    best.print();
}