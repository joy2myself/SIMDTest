#include <benchmark/benchmark.h>
#include "../../../include/core/std_simd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_std_simdLoad(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  std_simd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Aligned<std_simd_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_std_simdLoad)->Arg(1);