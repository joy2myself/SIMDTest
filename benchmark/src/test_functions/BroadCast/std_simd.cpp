#include <benchmark/benchmark.h>
#include "../../../include/core/std_simd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_std_simdBroadCast(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  std_simd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::BroadCast<std_simd_t_v_native<ElemType>, ElemType>(0.357f);
}
BENCHMARK(BM_std_simdBroadCast)->Arg(1);