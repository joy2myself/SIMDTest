#include <benchmark/benchmark.h>
#include "../../../include/core/vc_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_vcLen(benchmark::State& state) {
  for (auto _ : state)
    details::Len<vc_t_v_native<ElemType>, ElemType>();
}
BENCHMARK(BM_vcLen)->Arg(1);