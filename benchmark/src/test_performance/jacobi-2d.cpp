#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"

using ElemType = int32_t;

#define ARRLENGTH 32
#define TSTEPS    16

///////////////////////parameters initialization///////////////////////

const int n = ARRLENGTH;
const int tsteps = TSTEPS;
const std::size_t ITERATION = 5000;

ElemType A[ARRLENGTH][ARRLENGTH];
ElemType B[ARRLENGTH][ARRLENGTH];

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

/////////////////////////versiones of scalar///////////////////////////

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

///////////////////////versiones of simd///////////////////////////////

template<typename Vec, typename Tp> struct JACOBI_2D_SIMD
{
  void kernel_jacobi_2d(int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH])
  {
    std::size_t len = details::Len<Vec, Tp>();
    int t, i, j;
    Vec Aj_1, Aj, Aj1, Ai_1j, Ai1j;
    Vec Bj_1, Bj, Bj1, Bi_1j, Bi1j;
    for (t = 0; t < tsteps; t++)
    {
      for (i = 1; i < n - 1; i++)
      {
        for (j = 1; j < n - 1; j += len)
        {
          details::Load_Unaligned(Aj_1, &A[i][j - 1]);
          details::Load_Unaligned(Aj, &A[i][j]);
          details::Load_Unaligned(Aj1, &A[i][j + 1]);
          details::Load_Unaligned(Ai_1j, &A[i - 1][j]);
          details::Load_Unaligned(Ai1j, &A[i + 1][j]);

          Bj = details::BroadCast<Vec, Tp>(Tp(0.2)) * (Aj + Aj_1 + Aj1 + Ai1j + Ai_1j);

          details::Store_Unaligned<Vec, Tp>(Bj, &B[i][j]);
        }
      }
      for (i = 1; i < n - 1; i++)
      {
        for (j = 1; j < n - 1; j += len)
        {

          details::Load_Unaligned(Bj_1, &B[i][j - 1]);
          details::Load_Unaligned(Bj, &B[i][j]);
          details::Load_Unaligned(Bj1, &B[i][j + 1]);
          details::Load_Unaligned(Bi_1j, &B[i - 1][j]);
          details::Load_Unaligned(Bi1j, &B[i + 1][j]);

          Aj = details::BroadCast<Vec, Tp>(Tp(0.2)) * (Bj + Bj_1 + Bj1 + Bi1j + Bi_1j);

          details::Store_Unaligned<Vec, Tp>(Aj, &A[i][j]);
        }
      }
    }
  }
  void operator()(int tsteps, int n, ElemType A[ARRLENGTH][ARRLENGTH], ElemType B[ARRLENGTH][ARRLENGTH])
  {
    kernel_jacobi_2d(tsteps, n, A, B);
  }
};

//////////////bench function///////////////////////////////////////////

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]() { func(tsteps, n, A, B); });
}

int main()
{

  Initial();

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("JACOBI_2D_TEST_NATIVE").unit("JACOBI_2D_NATIVE").relative(true);
  b_native.performanceCounters(true);

  bench<JACOBI_2D_SCALAR>(b_native, "scalar");
  bench<JACOBI_2D_SIMD<scalar_t_v<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<JACOBI_2D_SIMD<array_t_v<ElemType, LEN>, ElemType>>(b_native, "array");
  bench<JACOBI_2D_SIMD<xsimd_t_v_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<JACOBI_2D_SIMD<std_simd_t_v_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<JACOBI_2D_SIMD<vcl_t_v_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<JACOBI_2D_SIMD<highway_t_v_native<ElemType>, ElemType>>(b_native, "highway");
  bench<JACOBI_2D_SIMD<nsimd_t_v_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<JACOBI_2D_SIMD<tsimd_t_v_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<JACOBI_2D_SIMD<eve_t_v_native<ElemType>, ElemType>>(b_native, "eve");
  bench<JACOBI_2D_SIMD<mipp_t_v_native<ElemType>, ElemType>>(b_native, "mipp"); // TODO: FIX
  // bench<JACOBI_2D_SIMD<vc_t_v_native<ElemType>, ElemType>>(b_native, "vc");

  return 0;
}
