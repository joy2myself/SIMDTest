#include <benchmark/benchmark.h>
#include "../../../include/core/nsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_nsimdBroadCast(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  nsimd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::BroadCast<nsimd_t_v_native<ElemType>, ElemType>(0.357f);
}
BENCHMARK(BM_nsimdBroadCast)->Arg(1);