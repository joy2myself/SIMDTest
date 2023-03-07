#include <benchmark/benchmark.h>
#include "../../../include/core/eve_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_eveLoad(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  eve_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Aligned<eve_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_eveLoad)->Arg(1);