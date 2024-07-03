#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>

using ElemType = int32_t;

#define ARRLENGTH 32
#define TSTEPS    16

const int n = ARRLENGTH;
const int tsteps = TSTEPS;
const std::size_t ITERATION = 5000;

ElemType A[ARRLENGTH][ARRLENGTH];
ElemType B[ARRLENGTH][ARRLENGTH];

void test_scalar(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);
void test_vc(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);
void test_tsimd(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
#endif
void test_highway(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);
void test_mipp(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);
void test_eve(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);
void test_xsimd(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH]);

void Initial()
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
    {
      A[i][j] = ((ElemType)i * (j + 2) + 2) / n;
      B[i][j] = ((ElemType)i * (j + 3) + 3) / n;
    }
}

int main()
{
    Initial();

    ankerl::nanobench::Bench b_native;
    b_native.title("jacobi2d_TEST_NATIVE").unit("jacobi2d_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    test_scalar(b_native, tsteps, n, A, B);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, tsteps, n, A, B);
    // #endif
    
    test_std_simd(b_native, tsteps, n, A, B);
    test_vc(b_native, tsteps, n, A, B);
    test_highway(b_native, tsteps, n, A, B);
    test_tsimd(b_native, tsteps, n, A, B);
    test_mipp(b_native, tsteps, n, A, B);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, tsteps, n, A, B);
    #endif

    if (PLACE_HOLDER){
      test_xsimd(b_native, tsteps, n, A, B);
      test_eve(b_native, tsteps, n, A, B);
    }
}