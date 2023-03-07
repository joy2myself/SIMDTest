#include <benchmark/benchmark.h>
#include "../../../include/core/mipp_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_mippLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<mipp_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_mippLen)->Arg(1);