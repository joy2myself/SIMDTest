#include <benchmark/benchmark.h>
#include "../../../include/core/std_simd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_std_simdMul(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{3.142f};
  std_simd_t_v_native<ElemType> rval, lval;
  details::Load_Aligned<std_simd_t_v_native<ElemType>, ElemType>(rval, Arr);
  details::Load_Aligned<std_simd_t_v_native<ElemType>, ElemType>(lval, Arr);
  for (auto _ : state)
    rval * lval;
}
BENCHMARK(BM_std_simdMul)->Arg(1);