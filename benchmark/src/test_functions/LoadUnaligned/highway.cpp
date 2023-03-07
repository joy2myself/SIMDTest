#include <benchmark/benchmark.h>
#include "../../../include/core/highway_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_highwayLoadU(benchmark::State& state) {
  ElemType Arr[Len]{0};
  highway_t_v_native<ElemType> v;
  for (auto _ : state)
    details::Load_Unaligned<highway_t_v_native<ElemType>, ElemType>(v, Arr);
}
BENCHMARK(BM_highwayLoadU)->Arg(1);