#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>

using ElemType = float;

const std::size_t ARRLENGTH = 512;
const std::size_t LEN = 4;
const std::size_t ITERATION = 5000000;

alignas(32) ElemType r[ARRLENGTH] = { 0 };
alignas(32) ElemType g[ARRLENGTH] = { 0 };
alignas(32) ElemType b[ARRLENGTH] = { 0 };
alignas(32) ElemType ya[ARRLENGTH] = { 0 };
alignas(32) ElemType ua[ARRLENGTH] = { 0 };
alignas(32) ElemType va[ARRLENGTH] = { 0 };

void test_scalar(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
void test_vc(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
#endif
void test_highway(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
void test_eve(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va);

void Initial()
{
  for (int i = 0; i < ARRLENGTH; i++)
  {
    r[i] = random() % 256;
    g[i] = random() % 256;
    b[i] = random() % 256;
  }
}

int main()
{
    Initial();

    ankerl::nanobench::Bench b_native;
    b_native.title("RGB2YUV_TEST_NATIVE").unit("RGB2YUV_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    test_scalar(b_native, r, g, b, ya, ua, va);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, r, g, b, ya, ua, va);
    // #endif
    
    test_std_simd(b_native, r, g, b, ya, ua, va);
    test_vc(b_native, r, g, b, ya, ua, va);
    test_highway(b_native, r, g, b, ya, ua, va);
    test_tsimd(b_native, r, g, b, ya, ua, va);
    test_mipp(b_native, r, g, b, ya, ua, va);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, r, g, b, ya, ua, va);
    #endif

    if (PLACE_HOLDER){
      test_xsimd(b_native, r, g, b, ya, ua, va);
      test_eve(b_native, r, g, b, ya, ua, va);
    }
}