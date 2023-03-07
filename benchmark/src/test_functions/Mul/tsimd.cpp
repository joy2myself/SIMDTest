#include <benchmark/benchmark.h>
#include "../../../include/core/tsimd_core.h"
using ElemType = float;
const size_t Len = 256;

static void BM_tsimdMul(benchmark::State& state) {
  alignas(32) ElemType Arr[Len]{3.142f};
  tsimd_t_v_native<ElemType> rval, lval;
  details::Load_Aligned<tsimd_t_v_native<ElemType>, ElemType>(rval, Arr);
  details::Load_Aligned<tsimd_t_v_native<ElemType>, ElemType>(lval, Arr);
  for (auto _ : state)
    rval * lval;
}
BENCHMARK(BM_tsimdMul)->Arg(1);