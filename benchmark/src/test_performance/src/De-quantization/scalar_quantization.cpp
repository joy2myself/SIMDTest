#include <cstdlib>
#include <nanobench.h>
using ElemType = int32_t;

#if defined(__SSE2__)
const std::size_t ARRLENGTH = 64;
#else
const std::size_t ARRLENGTH = 128;
#endif
const std::size_t LEN = 4;
const std::size_t ITERATION = 500000;

struct DE_QUAN_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  void dequant_8x8(ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
  {
    const ElemType i_mf = i_qp % 6;
    const ElemType i_qbits = i_qp / 6 - 6;

    if (i_qbits >= 0)
    {
      for (int i = 0; i < ARRLENGTH; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i]) << i_qbits;
    }
    else
    {
      const int f = 1 << (-i_qbits - 1);
      for (int i = 0; i < ARRLENGTH; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i] + f) >> (-i_qbits);
    }
  }
  void operator()(ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
  {
    dequant_8x8(dct, dequant_mf, i_qp, res);
  }
#pragma GCC pop_options
#else
  void dequant_8x8(ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
  {
    const ElemType i_mf = i_qp % 6;
    const ElemType i_qbits = i_qp / 6 - 6;

    if (i_qbits >= 0)
    {
      for (int i = 0; i < ARRLENGTH; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i]) << i_qbits;
    }
    else
    {
      const ElemType f = 1 << (-i_qbits - 1);
      for (int i = 0; i < ARRLENGTH; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i] + f) >> (-i_qbits);
    }
  }
  void operator()(ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
  {
    dequant_8x8(dct, dequant_mf, i_qp, res);
  }
#endif
};

void test_scalar(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
{ 
  DE_QUAN_SCALAR f;
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    f(dct, dequant_mf, i_qp, res);
    ankerl::nanobench::doNotOptimizeAway(f);});
}