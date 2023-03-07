#include <benchmark/benchmark.h>
#include "../../../include/core/vcl_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_vclLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<vcl_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_vclLen)->Arg(1);