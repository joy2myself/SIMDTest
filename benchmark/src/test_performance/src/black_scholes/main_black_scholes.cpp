#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>

using ElemType = float;

const std::size_t ARRLENGTH = 128 * 1024;
const std::size_t LEN = 4;
const std::size_t ITERATION = 5;

ElemType *S = new ElemType[ARRLENGTH];
ElemType *X = new ElemType[ARRLENGTH];
ElemType *T = new ElemType[ARRLENGTH];
ElemType *r = new ElemType[ARRLENGTH];
ElemType *v = new ElemType[ARRLENGTH];
ElemType *result = new ElemType[ARRLENGTH];

void test_scalar(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
void test_vc(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
#endif
void test_highway(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
void test_eve(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count);

void Initial()
{
  for (int i = 0; i < ARRLENGTH; ++i)
  {
    S[i] = 100; // stock price
    X[i] = 98;  // option strike price
    T[i] = 2;   // time (years)
    r[i] = .02; // risk-free interest rate
    v[i] = 5;   // volatility
  }
}

int main()
{
    Initial();

    ankerl::nanobench::Bench b_native;
    b_native.title("black_scholes_TEST_NATIVE").unit("black_scholes_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    test_scalar(b_native, S, X, T, r, v, result, ARRLENGTH);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, S, X, T, r, v, result, ARRLENGTH);
    // #endif
    
    test_std_simd(b_native, S, X, T, r, v, result, ARRLENGTH);
    test_vc(b_native, S, X, T, r, v, result, ARRLENGTH);
    test_tsimd(b_native, S, X, T, r, v, result, ARRLENGTH);
    test_mipp(b_native, S, X, T, r, v, result, ARRLENGTH);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, S, X, T, r, v, result, ARRLENGTH);
    #endif

    if (PLACE_HOLDER){
      test_highway(b_native, S, X, T, r, v, result, ARRLENGTH);
      test_xsimd(b_native, S, X, T, r, v, result, ARRLENGTH);
      test_eve(b_native, S, X, T, r, v, result, ARRLENGTH);
    }
}


