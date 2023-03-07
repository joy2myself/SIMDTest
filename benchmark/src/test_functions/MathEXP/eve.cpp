#include <benchmark/benchmark.h>
#include "../../../include/core/eve_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_eveExp(benchmark::State& state) {
  ElemType Arr[Len]{3.1384f};
  eve_t_v_native<ElemType> v;
  details::Load_Unaligned<eve_t_v_native<ElemType>, ElemType>(v, Arr);
  for (auto _ : state)
    details::Exp<ElemType>(v);
}
BENCHMARK(BM_eveExp)->Arg(1);