#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"

using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t ARRLENGTH = 512;
const std::size_t LEN = 4;
const std::size_t ITERATION = 5000000;

ElemType r[ARRLENGTH] = { 0 };
ElemType g[ARRLENGTH] = { 0 };
ElemType b[ARRLENGTH] = { 0 };
ElemType ya[ARRLENGTH] = { 0 };
ElemType ua[ARRLENGTH] = { 0 };
ElemType va[ARRLENGTH] = { 0 };

void Initial()
{
  for (int i = 0; i < ARRLENGTH; i++)
  {
    r[i] = random() % 256;
    g[i] = random() % 256;
    b[i] = random() % 256;
  }
}

/////////////////////////versiones of scalar///////////////////////////

struct RGB2YUV_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  void rgb2yuv(ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va)
  {
    for (int i = 0; i < ARRLENGTH; i++)
    {
      ya[i] = 0.299f * ra[i] + 0.584f * ga[i] + 0.114f * ba[i];
      ua[i] = -0.14713f * ra[i] - 0.28886f * ga[i] + 0.436f * ba[i];
      va[i] = 0.615f * ra[i] - 0.51499f * ga[i] - 0.10001f * ba[i];
    }
  }
  void operator()(ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va)
  {
    rgb2yuv(ra, ga, ba, ya, ua, va);
  }
#pragma GCC pop_options
#else
  void rgb2yuv(ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va)
  {
    for (int i = 0; i < ARRLENGTH; i++)
    {
      ya[i] = 0.299f * ra[i] + 0.584f * ga[i] + 0.114f * ba[i];
      ua[i] = -0.14713f * ra[i] - 0.28886f * ga[i] + 0.436f * ba[i];
      va[i] = 0.615f * ra[i] - 0.51499f * ga[i] - 0.10001f * ba[i];
    }
  }
  void operator()(ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va)
  {
    rgb2yuv(ra, ga, ba, ya, ua, va);
  }
#endif
};

///////////////////////versiones of simd///////////////////////////////

template<typename Vec, typename Tp> struct RGB2YUV_SIMD
{
  void rgb2yuv(Tp *ra, Tp *ga, Tp *ba, Tp *ya, Tp *ua, Tp *va)
  {
    Vec a, b, c, y, u, v;
    auto len = details::Len<Vec, Tp>();
    for (int i = 0; i < ARRLENGTH; i += len)
    {
      details::Load_Aligned(a, &ra[i]);
      details::Load_Aligned(b, &ga[i]);
      details::Load_Aligned(c, &ba[i]);
      y = details::BroadCast<Vec, Tp>(Tp(0.299)) * a + details::BroadCast<Vec, Tp>(Tp(0.584)) * b +
          details::BroadCast<Vec, Tp>(Tp(0.114)) * c;
      u = details::BroadCast<Vec, Tp>(Tp(-0.14713)) * a - details::BroadCast<Vec, Tp>(Tp(0.28886)) * b +
          details::BroadCast<Vec, Tp>(Tp(0.436)) * c;
      v = details::BroadCast<Vec, Tp>(Tp(0.615)) * a - details::BroadCast<Vec, Tp>(Tp(0.51499)) * b -
          details::BroadCast<Vec, Tp>(Tp(0.10001)) * c;
      details::Store_Aligned(y, &ya[i]);
      details::Store_Aligned(u, &ua[i]);
      details::Store_Aligned(v, &va[i]);
    }
  }

  void operator()(Tp *ra, Tp *ga, Tp *ba, Tp *ya, Tp *ua, Tp *va)
  {
    rgb2yuv(ra, ga, ba, ya, ua, va);
  }
};

//////////////bench function///////////////////////////////////////////

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]() {
    func(r, g, b, ya, ua, va);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
}
/*

details::Mul

*/
int main(int argc, char *argv[])
{

  Initial();

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("RGB2YUV_TEST_NATIVE").unit("RGB2YUV_NATIVE").warmup(100).relative(true);
  b_native.performanceCounters(true);

  bench<RGB2YUV_SCALAR>(b_native, "scalar");
  bench<RGB2YUV_SIMD<scalar_t_v<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<RGB2YUV_SIMD<array_t_v<ElemType, LEN>, ElemType>>(b_native, "array");
  bench<RGB2YUV_SIMD<xsimd_t_v_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<RGB2YUV_SIMD<std_simd_t_v_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<RGB2YUV_SIMD<vcl_t_v_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<RGB2YUV_SIMD<highway_t_v_native<ElemType>, ElemType>>(b_native, "highway");
  bench<RGB2YUV_SIMD<nsimd_t_v_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<RGB2YUV_SIMD<tsimd_t_v_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<RGB2YUV_SIMD<eve_t_v_native<ElemType>, ElemType>>(b_native, "eve");
  bench<RGB2YUV_SIMD<mipp_t_v_native<ElemType>, ElemType>>(b_native, "mipp");
  bench<RGB2YUV_SIMD<vc_t_v_native<ElemType>, ElemType>>(b_native, "vc");

  return 0;
}