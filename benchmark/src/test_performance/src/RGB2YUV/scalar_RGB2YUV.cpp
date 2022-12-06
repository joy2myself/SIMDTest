#include <cstdlib>
#include <nanobench.h>
using ElemType = float;

const std::size_t ARRLENGTH = 256;
const std::size_t LEN = 4;
const std::size_t ITERATION = 500000;

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


void test_scalar(ankerl::nanobench::Bench &bench, ElemType *ra, ElemType *ga, ElemType *ba, ElemType *ya, ElemType *ua, ElemType *va)
{ 
  RGB2YUV_SCALAR f;
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    f(ra, ga, ba, ya, ua, va);
    ankerl::nanobench::doNotOptimizeAway(f);});
}