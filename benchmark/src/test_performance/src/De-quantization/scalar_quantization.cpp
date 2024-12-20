#include <cstdlib>
#include <nanobench.h>
using ElemType = int32_t;

const std::size_t ARRLENGTH = 256;
const std::size_t LEN = 4;
const std::size_t ITERATION = 500000;

struct DE_QUAN_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  void dequant_8x8(ElemType dct[64], ElemType dequant_mf[6][64], ElemType i_qp, ElemType res[64])
  {
    const ElemType i_mf = i_qp % 6;
    const ElemType i_qbits = i_qp / 6 - 6;

    if (i_qbits >= 0)
    {
      for (int i = 0; i < 64; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i]) << i_qbits;
    }
    else
    {
      const int f = 1 << (-i_qbits - 1);
      for (int i = 0; i < 64; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i] + f) >> (-i_qbits);
    }
  }
  void operator()(ElemType dct[64], ElemType dequant_mf[6][64], ElemType i_qp, ElemType res[64])
  {
    dequant_8x8(dct, dequant_mf, i_qp, res);
  }
#pragma GCC pop_options
#else
  void dequant_8x8(ElemType dct[64], ElemType dequant_mf[6][64], ElemType i_qp, ElemType res[64])
  {
    const ElemType i_mf = i_qp % 6;
    const ElemType i_qbits = i_qp / 6 - 6;

    if (i_qbits >= 0)
    {
      for (int i = 0; i < 64; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i]) << i_qbits;
    }
    else
    {
      const ElemType f = 1 << (-i_qbits - 1);
      for (int i = 0; i < 64; i++)
        res[i] = (dct[i] * dequant_mf[i_mf][i] + f) >> (-i_qbits);
    }
  }
  void operator()(ElemType dct[64], ElemType dequant_mf[6][64], ElemType i_qp, ElemType res[64])
  {
    dequant_8x8(dct, dequant_mf, i_qp, res);
  }
#endif
};

// 使用 nanobench 对标量实现进行性能测试
void test_scalar(ankerl::nanobench::Bench &bench, ElemType dct[64], ElemType dequant_mf[6][64], ElemType i_qp, ElemType res[64])
{ 
  DE_QUAN_SCALAR f;
  // 配置 nanobench 的性能测试，指定最少迭代次数，执行标量实现并记录性能结果
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    f(dct, dequant_mf, i_qp, res);
    ankerl::nanobench::doNotOptimizeAway(f);});
}