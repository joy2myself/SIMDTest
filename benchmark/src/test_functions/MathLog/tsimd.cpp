#include <benchmark/benchmark.h>
#include "../../../include/core/tsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_tsimdLog(benchmark::State& state) {
  ElemType Arr[Len]{3.1384f};
  tsimd_t_v_native<ElemType> v;
  details::Load_Unaligned<tsimd_t_v_native<ElemType>, ElemType>(v, Arr);
  for (auto _ : state)
    details::Log<ElemType>(v);
}
BENCHMARK(BM_tsimdLog)->Arg(1);