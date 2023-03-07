#include <benchmark/benchmark.h>
#include "../../../include/core/std_simd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_std_simdLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<std_simd_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_std_simdLen)->Arg(1);