#include "../../../../include/core/std_simd_core.h"
#include <nanobench.h>
using ElemType = float;

const std::size_t ARRLENGTH = 256;
const std::size_t LEN = 4;
const std::size_t ITERATION = 500000;

template<typename Vec, typename Tp> struct AXPY_SIMD
{
  void operator()(Tp a, Tp *x, Tp *y, Tp *res)
  {
    auto len = details::Len<Vec, Tp>();
    std::size_t vec_size = ARRLENGTH - ARRLENGTH % len;
    Vec x_simd, y_simd, res_simd;
    for (std::size_t i = 0; i < vec_size; i += len)
    {
      details::Load_Aligned(x_simd, &x[i]);
      details::Load_Aligned(y_simd, &y[i]);
      res_simd = details::BroadCast<Vec, Tp>(a) * x_simd + y_simd;
      details::Store_Aligned(res_simd, &res[i]);
    }
    for (std::size_t i = vec_size; i < ARRLENGTH; ++i)
    {
      res[i] = a * x[i] + y[i];
    }
  }
};

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res)
{
#if defined(USE_PLCT_SIMD)
  AXPY_SIMD<std_simd_t_v_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("plct_simd", [&]() {
    func(a, x, y, res);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#else
  AXPY_SIMD<std_simd_t_v_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("std_simd", [&]() {
    func(a, x, y, res);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#endif
}