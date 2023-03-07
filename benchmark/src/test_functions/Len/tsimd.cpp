#include <benchmark/benchmark.h>
#include "../../../include/core/tsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_tsimdLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<tsimd_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_tsimdLen)->Arg(1);