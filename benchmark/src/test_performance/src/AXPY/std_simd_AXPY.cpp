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
      details::Load_Unaligned(x_simd, &x[i]);
      details::Load_Unaligned(y_simd, &y[i]);
      res_simd = details::BroadCast<Vec, Tp>(a) * x_simd + y_simd;
      details::Store_Unaligned(res_simd, &res[i]);
    }
    for (std::size_t i = vec_size; i < ARRLENGTH; ++i)
    {
      res[i] = a * x[i] + y[i];
    }
  }
};

// 使用 nanobench 对simd实现进行性能测试
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
  // 配置 nanobench 的性能测试，指定最少迭代次数，执行simd实现并记录性能结果
  bench.minEpochIterations(ITERATION).run("std_simd", [&]() {
    func(a, x, y, res);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#endif
}