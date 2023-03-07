#include <benchmark/benchmark.h>
#include "../../../include/core/eve_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_eveBroadCast(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  eve_t_v_native<ElemType> v;
  for (auto _ : state)
    details::BroadCast<eve_t_v_native<ElemType>, ElemType>(0.357f);
}
BENCHMARK(BM_eveBroadCast)->Arg(1);