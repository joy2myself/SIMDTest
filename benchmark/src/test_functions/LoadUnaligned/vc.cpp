#include <benchmark/benchmark.h>
#include "../../../include/core/vc_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_vcLoadU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  vc_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Unaligned<vc_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_vcLoadU)->Arg(1);