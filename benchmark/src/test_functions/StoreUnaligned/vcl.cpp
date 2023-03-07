#include <benchmark/benchmark.h>
#include "../../../include/core/vcl_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_vclStoreU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  ElemType Arr1[Len]{0};
  vcl_t_v_native<ElemType> v;
  details::Load_Unaligned<vcl_t_v_native<ElemType>, ElemType>(v, Arr);
  for (auto _ : state)
    details::Store_Unaligned<vcl_t_v_native<ElemType>, ElemType>(v, Arr1);
}
BENCHMARK(BM_vclStoreU)->Arg(1);