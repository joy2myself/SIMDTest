#include <benchmark/benchmark.h>
#include "../../../include/core/std_simd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_std_simdLoadU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  std_simd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Unaligned<std_simd_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_std_simdLoadU)->Arg(1);