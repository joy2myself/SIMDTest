#include <benchmark/benchmark.h>
#include "../../../include/core/nsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_nsimdFabs(benchmark::State& state) {
  ElemType Arr[Len]{3.1384f};
  nsimd_t_v_native<ElemType> v;
  details::Load_Unaligned<nsimd_t_v_native<ElemType>, ElemType>(v, Arr);
  for (auto _ : state)
    details::Fabs<ElemType>(v);
}
BENCHMARK(BM_nsimdFabs)->Arg(1);