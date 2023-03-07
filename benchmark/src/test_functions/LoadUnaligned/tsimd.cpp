#include <benchmark/benchmark.h>
#include "../../../include/core/tsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_tsimdLoadU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  tsimd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Unaligned<tsimd_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_tsimdLoadU)->Arg(1);