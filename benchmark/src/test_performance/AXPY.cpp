#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"

using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t ARRLENGTH = 256;
const std::size_t LEN = 4;
const std::size_t ITERATION = 5000000;

ElemType a;
ElemType x[ARRLENGTH]{0};
ElemType y[ARRLENGTH]{0};
ElemType res[ARRLENGTH]{0};

void Initial()
{
  a = ElemType(rand());
  for (size_t i = 0; i < ARRLENGTH; ++i)
  {
    x[i] = ElemType(rand());
    y[i] = ElemType(rand());
    res[i] = 0.f;
  }
}

/////////////////////////versiones of scalar///////////////////////////

struct AXPY_SCALAR
{
#ifdef OpenAutoOptimize
  __attribute__((optimize("O2", "tree-vectorize"))) void operator()(ElemType a, ElemType *b, ElemType *c, ElemType *res)
  {
    for (int i = 0; i < ARRLENGTH; ++i)
    {
      res[i] = a * b[i] + c[i];
    }
  }
#else
  void operator()(ElemType a, ElemType *b, ElemType *c, ElemType *res)
  {
    for (int i = 0; i < ARRLENGTH; ++i)
    {
      res[i] = a * b[i] + c[i];
    }
  }
#endif
};

///////////////////////versiones of simd///////////////////////////////

template <typename Vec, typename Tp>
struct AXPY_SIMD
{
  void operator()(Tp a, Tp *x, Tp *y, Tp *res)
  {
    auto len = details::Len<Vec, Tp>();
    std::size_t vec_size = ARRLENGTH - ARRLENGTH % len;
    Vec x_simd, y_simd, res_simd;
    for (std::size_t i = 0; i < vec_size; i += len)
    {
      details::Load_Aligned(x_simd, &x[i]);
      details::Load_Aligned(y_simd, &y[i]);
      res_simd = details::BroadCast<Vec, Tp>(a) * x_simd + y_simd;
      details::Store_Aligned(res_simd, &res[i]);
    }
    for (std::size_t i = vec_size; i < ARRLENGTH; ++i)
    {
      res[i] = a * x[i] + y[i];
    }
  }
};

//////////////bench function///////////////////////////////////////////

template <typename F>
void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]()
                                          {
    func(a, x, y, res);
    ankerl::nanobench::doNotOptimizeAway(func); });
}

int main()
{

  Initial();

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("AXPY_TEST_NATIVE").unit("AXPY_NATIVE").warmup(100).relative(true);
  b_native.performanceCounters(true);

  bench<AXPY_SCALAR>(b_native, "scalar");
  bench<AXPY_SIMD<scalar_t_v<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<AXPY_SIMD<array_t_v<ElemType, LEN>, ElemType>>(b_native, "array");
  bench<AXPY_SIMD<xsimd_t_v_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<AXPY_SIMD<std_simd_t_v_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<AXPY_SIMD<vcl_t_v_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<AXPY_SIMD<highway_t_v_native<ElemType>, ElemType>>(b_native, "highway");
  bench<AXPY_SIMD<nsimd_t_v_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<AXPY_SIMD<tsimd_t_v_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<AXPY_SIMD<eve_t_v_native<ElemType>, ElemType>>(b_native, "eve");
  bench<AXPY_SIMD<mipp_t_v_native<ElemType>, ElemType>>(b_native, "mipp");
  bench<AXPY_SIMD<vc_t_v_native<ElemType>, ElemType>>(b_native, "vc");
  return 0;
}
