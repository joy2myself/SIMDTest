#include <benchmark/benchmark.h>
#include "../../../include/core/vcl_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_vclLoadU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  vcl_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Unaligned<vcl_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_vclLoadU)->Arg(1);