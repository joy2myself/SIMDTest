#include <benchmark/benchmark.h>
#include "../../../include/core/eve_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_eveLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<eve_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_eveLen)->Arg(1);