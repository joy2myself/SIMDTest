#include <benchmark/benchmark.h>
#include "../../../include/core/xsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_xsimdSqrt(benchmark::State& state) {
  ElemType Arr[Len]{3.1384f};
  xsimd_t_v_native<ElemType> v;
  details::Load_Unaligned<xsimd_t_v_native<ElemType>, ElemType>(v, Arr);
  for (auto _ : state)
    details::Sqrt<ElemType>(v);
}
BENCHMARK(BM_xsimdSqrt)->Arg(1);