#include <benchmark/benchmark.h>
#include "../../../include/core/xsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_xsimdLoadU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  xsimd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Unaligned<xsimd_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_xsimdLoadU)->Arg(1);