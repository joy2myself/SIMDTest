#include <benchmark/benchmark.h>
#include "../../../include/core/std_simd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_std_simdLog(benchmark::State& state) {
  ElemType Arr[Len]{3.1384f};
  std_simd_t_v_native<ElemType> v;
  details::Load_Unaligned<std_simd_t_v_native<ElemType>, ElemType>(v, Arr);
  for (auto _ : state)
    details::Log<ElemType>(v);
}
BENCHMARK(BM_std_simdLog)->Arg(1);