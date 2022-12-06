#include <cstdlib>
#include <nanobench.h>

using ElemType = int32_t;

#define ARRLENGTH 32
#define TSTEPS    16

///////////////////////parameters initialization///////////////////////

const int n = ARRLENGTH;
const int tsteps = TSTEPS;
const std::size_t ITERATION = 5000;

struct JACOBI_2D_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  void kernel_jacobi_2d(int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH])
  {
    int t, i, j;
    for (t = 0; t < tsteps; t++)
    {
      for (i = 1; i < n - 1; i++)
        for (j = 1; j < n - 1; j++)
          B[i][j] = static_cast<ElemType>(0.2) * (A[i][j] + A[i][j - 1] + A[i][1 + j] + A[1 + i][j] + A[i - 1][j]);
      for (i = 1; i < n - 1; i++)
        for (j = 1; j < n - 1; j++)
          A[i][j] = static_cast<ElemType>(0.2) * (B[i][j] + B[i][j - 1] + B[i][1 + j] + B[1 + i][j] + B[i - 1][j]);
    }
  }
  void operator()(int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH])
  {
    kernel_jacobi_2d(tsteps, n, A, B);
  }
#pragma GCC pop_options
#else
  void kernel_jacobi_2d(int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH])
  {
    int t, i, j;
    for (t = 0; t < tsteps; t++)
    {
      for (i = 1; i < n - 1; i++)
        for (j = 1; j < n - 1; j++)
          B[i][j] = static_cast<ElemType>(0.2) * (A[i][j] + A[i][j - 1] + A[i][1 + j] + A[1 + i][j] + A[i - 1][j]);
      for (i = 1; i < n - 1; i++)
        for (j = 1; j < n - 1; j++)
          A[i][j] = static_cast<ElemType>(0.2) * (B[i][j] + B[i][j - 1] + B[i][1 + j] + B[1 + i][j] + B[i - 1][j]);
    }
  }
  void operator()(int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH])
  {
    kernel_jacobi_2d(tsteps, n, A, B);
  }
#endif
};


void test_scalar(ankerl::nanobench::Bench &bench, int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH])
{ 
  JACOBI_2D_SCALAR f;
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    f(tsteps, n, A, B);
    ankerl::nanobench::doNotOptimizeAway(f);});
}