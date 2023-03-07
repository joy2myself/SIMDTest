#include <benchmark/benchmark.h>
#include "../../../include/core/vcl_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_vclStore(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{0};
  alignas(32) ElemType Arr1[Len]{0};
  vcl_t_v_native<ElemType> v;
  details::Load_Aligned<vcl_t_v_native<ElemType>, ElemType>(v, Arr);
  for (auto _ : state)
    details::Store_Aligned<vcl_t_v_native<ElemType>, ElemType>(v, Arr1);
}
BENCHMARK(BM_vclStore)->Arg(1);