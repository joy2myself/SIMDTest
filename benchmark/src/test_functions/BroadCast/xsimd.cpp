#include <benchmark/benchmark.h>
#include "../../../include/core/xsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_xsimdBroadCast(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  xsimd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::BroadCast<xsimd_t_v_native<ElemType>, ElemType>(0.357f);
}
BENCHMARK(BM_xsimdBroadCast)->Arg(1);