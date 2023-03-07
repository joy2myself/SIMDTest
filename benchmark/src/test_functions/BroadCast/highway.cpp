#include <benchmark/benchmark.h>
#include "../../../include/core/highway_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_highwayBroadCast(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  highway_t_v_native<ElemType> v;
  for (auto _ : state)
    details::BroadCast<highway_t_v_native<ElemType>, ElemType>(0.357f);
}
BENCHMARK(BM_highwayBroadCast)->Arg(1);