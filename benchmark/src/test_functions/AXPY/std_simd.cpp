#include <benchmark/benchmark.h>
#include "../../../include/core/std_simd_core.h"
using ElemType = float;
const size_t Len = 256;
const std::size_t ARRLENGTH = 128;

void Initial(ElemType a,ElemType* x,ElemType* y,ElemType* res);
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

static void BM_std_simdAXPY(benchmark::State& state) {
  ElemType a;
  alignas(32) ElemType x[ARRLENGTH]{ 0 };
  alignas(32) ElemType y[ARRLENGTH]{ 0 };
  alignas(32) ElemType res[ARRLENGTH]{ 0 };
  Initial(a, x, y, res);
  for (auto _ : state)
  AXPY_SIMD<std_simd_t_v_native<ElemType>, ElemType>{}(a, x, y, res);
}
BENCHMARK(BM_std_simdAXPY)->Arg(1);