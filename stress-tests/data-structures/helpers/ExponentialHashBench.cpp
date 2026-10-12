// Manual Linux/GCC benchmark; see doc/exponential-hash.md.
#include <bits/stdc++.h>
#include <sched.h>
using namespace std;
using ll=long long; using ull=unsigned long long;
#include "../../../content/number-theory/ModMulLL.h"
#define FOR(i,a,b) for(int i=(a);i<(b);++i)
#include "../../../content/number-theory/FixedModPow.h"
constexpr ull P=(1ULL<<61)-1;
volatile ull sink=0;
constexpr int N=4096, LOOPS=256;
ull data_[N];

template<int Method,ull Fixed> inline ull op(ull a,ull b,ull runtime){
    ull m=Fixed?Fixed:runtime;
    if constexpr(Method==0)return mmul(a,b,m);
    if constexpr(Method==1)return (ull)((__uint128_t)a*b%m);
    if constexpr(Method==2)return (ull)((__int128_t)a*b%m);
    if constexpr(Method==3){auto z=(__uint128_t)a*b;ull r=(ull)z&P;r+=(ull)(z>>61);return r>=P?r-P:r;}
    if constexpr(Method==4)return a*b%m;
}
template<int Method,ull Fixed,int Lanes>
__attribute__((noinline,noipa)) ull run_mul(ull m){
    ull x[8]{17,29,31,43,53,67,79,89};
    for(int k=0;k<LOOPS;++k)for(int i=0;i<N;i+=Lanes)
        for(int j=0;j<Lanes;++j)x[j]=op<Method,Fixed>(x[j],data_[i+j],m);
    return accumulate(x,x+Lanes,0ULL);
}
struct Case{string name;function<ull()> fn; vector<double> ns;ull answer;};
void measure(vector<Case>& cases,int ops){
    mt19937 rng(22);
    for(auto &c:cases){c.answer=c.fn();sink=c.answer;}
    for(int rep=0;rep<7;++rep){
        shuffle(cases.begin(),cases.end(),rng);
        for(auto &c:cases){
            auto begin=chrono::steady_clock::now();auto result=c.fn();auto end=chrono::steady_clock::now();
            if(result!=c.answer)abort();
            sink=result;
            c.ns.push_back(chrono::duration<double,nano>(end-begin).count()/ops);
        }
    }
    sort(cases.begin(),cases.end(),[](auto&a,auto&b){return a.name<b.name;});
    for(auto &c:cases){sort(c.ns.begin(),c.ns.end());cout<<c.name<<","<<fixed<<setprecision(3)<<c.ns[c.ns.size()/2]<<","<<c.answer<<"\n";}
}
template<ull Fixed> void group(const string &name,ull m,bool small=false){
    mt19937_64 rng(127);for(auto &x:data_)do x=rng()%m; while(gcd(x,m)!=1);
    vector<Case> cases;
#define ADD(ID,NAME,L) cases.push_back({name+"/"+NAME+"/"+#L, [=]{return run_mul<ID,Fixed,L>(m);},{},0})
    ADD(0,"long-double",1);ADD(0,"long-double",8);
    ADD(1,"uint128",1);ADD(1,"uint128",8);
    ADD(2,"int128",1);ADD(2,"int128",8);
    if(m==P){ADD(3,"mersenne",1);ADD(3,"mersenne",8);}
    if(small){ADD(4,"uint64",1);ADD(4,"uint64",8);}
#undef ADD
    measure(cases,N*LOOPS);
    for(int lane:{1,8}){
        optional<ull> ans;
        for(auto &c:cases)if(c.name.back()==char('0'+lane)){
            if(ans&&*ans!=c.answer){cerr<<"mismatch "<<c.name<<"\n";abort();}ans=c.answer;
        }
    }
}
ll signed_[N],positive_[N],target_[N],factor_[N];
template<int Mode,bool Multiply> __attribute__((noinline,noipa)) ull run_residue(){
    ll a=17;ull hit=0;
    for(int k=0;k<LOOPS;++k)for(int i=0;i<N;++i){
        if constexpr(Mode==0)a=((a+positive_[i])%(ll)P+(ll)P)%(ll)P;
        if constexpr(Mode==1){a+=positive_[i];if(a>=(ll)P)a-=P;}
        if constexpr(Mode==2)a=(a+signed_[i])%(ll)P;
        if constexpr(Mode==3){a+=signed_[i];if(a>=(ll)P)a-=P;if(a<=-(ll)P)a+=P;}
        if constexpr(Multiply){
            if((i&7)==0){
                if constexpr(Mode<2)a=(ll)((__uint128_t)a*factor_[i]%P);
                else a=(ll)((__int128_t)a*factor_[i]%(ll)P);
            }
        }
        if constexpr(Mode<2)hit+=(a==target_[i]);
        else hit+=((a-target_[i])%(ll)P==0);
    }
    return (ull)((a+(ll)P)%(ll)P)+hit;
}
void residue(){
    mt19937_64 rng(928);
    for(int i=0;i<N;++i){signed_[i]=(ll)(rng()%P);if(i&1)signed_[i]=-signed_[i];positive_[i]=(signed_[i]+(ll)P)%(ll)P;target_[i]=(ll)(rng()%P);factor_[i]=(ll)(rng()%P);}
    vector<Case> cases;
#define ADD(M,NAME,FLAG) cases.push_back({string("residue/")+NAME+"/"+#FLAG,[]{return run_residue<M,FLAG>();},{},0})
    ADD(0,"canonical-double-mod",false);ADD(0,"canonical-double-mod",true);
    ADD(1,"canonical-conditional",false);ADD(1,"canonical-conditional",true);
    ADD(2,"signed-remainder",false);ADD(2,"signed-remainder",true);
    ADD(3,"signed-conditional",false);ADD(3,"signed-conditional",true);
#undef ADD
    measure(cases,N*LOOPS);
    map<string,ull> refs;
    for(auto &c:cases){auto key=c.name.substr(c.name.rfind('/'));if(refs.count(key)&&refs[key]!=c.answer)abort();refs[key]=c.answer;}
}
template<int Method> ull binary_pow(ull b,ull e){
    ull r=1;
    for(;e;e>>=1,b=op<Method,P>(b,b,P))if(e&1)r=op<Method,P>(r,b,P);
    return r;
}
void powers(){
    mt19937_64 rng(613);for(auto &e:data_)e=rng()%(P-1);
    ull b=mpow(37,100000000003ULL,P);
    auto start=chrono::steady_clock::now();
    auto p8=make_unique<FixedModPow<P,8>>(b);
    auto middle=chrono::steady_clock::now();
    auto p16=make_unique<FixedModPow<P,16>>(b);
    auto finish=chrono::steady_clock::now();
    cerr<<"Table setup us: 8="<<chrono::duration<double,micro>(middle-start).count()
        <<", 16="<<chrono::duration<double,micro>(finish-middle).count()<<"\n";
    vector<Case> cases;
#define ADD(NAME,EXPR) cases.push_back({"power/" NAME,[&]{ull r=0;for(int k=0;k<32;++k)for(ull e:data_)r+=EXPR;return r;},{},0})
    ADD("binary-long-double",(binary_pow<0>(b,e)));
    ADD("binary-uint128",(binary_pow<1>(b,e)));
    ADD("binary-mersenne",(binary_pow<3>(b,e)));
    ADD("table-8",((*p8)(e)));
    ADD("table-16",((*p16)(e)));
#undef ADD
    measure(cases,N*32);
    for(auto &c:cases)if(c.answer!=cases[0].answer)abort();
}
int main(){
    cpu_set_t allowed;CPU_ZERO(&allowed);sched_getaffinity(0,sizeof(allowed),&allowed);
    int cpu=0;while(!CPU_ISSET(cpu,&allowed))++cpu;cpu_set_t pin;CPU_ZERO(&pin);CPU_SET(cpu,&pin);sched_setaffinity(0,sizeof(pin),&pin);
    cerr<<"CPU "<<cpu<<"; nanoseconds per multiplication or residue iteration; 1=dependent chain, 8=parallel chains\n";
    volatile ull runtime=P;
    group<0>("runtime-P",runtime);
    group<P>("constexpr-P",P);
    group<P-1>("constexpr-Pminus1",P-1);
    group<1000000007ULL>("constexpr-1e9+7",1000000007,true);
    residue();
    powers();
}
