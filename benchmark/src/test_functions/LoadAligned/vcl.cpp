#include <benchmark/benchmark.h>
#include "../../../include/core/vcl_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_vclLoad(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  vcl_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Aligned<vcl_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_vclLoad)->Arg(1);