#include <benchmark/benchmark.h>
#include "../../../include/core/highway_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_highwayLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<highway_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_highwayLen)->Arg(1);