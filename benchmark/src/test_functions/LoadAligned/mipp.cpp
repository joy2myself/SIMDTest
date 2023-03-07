#include <benchmark/benchmark.h>
#include "../../../include/core/mipp_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_mippLoad(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  mipp_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Aligned<mipp_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_mippLoad)->Arg(1);