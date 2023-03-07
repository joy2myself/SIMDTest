#include <benchmark/benchmark.h>
#include "../../../include/core/nsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_nsimdLoadU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  nsimd_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Unaligned<nsimd_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_nsimdLoadU)->Arg(1);