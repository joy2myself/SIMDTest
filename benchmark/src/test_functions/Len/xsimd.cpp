#include <benchmark/benchmark.h>
#include "../../../include/core/xsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_xsimdLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<xsimd_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_xsimdLen)->Arg(1);