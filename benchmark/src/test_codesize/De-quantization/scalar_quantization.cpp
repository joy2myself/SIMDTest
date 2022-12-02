#include "../../../include/core/eve_core.h"

using ElemType = int32_t;

///////////////////////parameters initialization////////////////////////

const std::size_t ITERATION = 5000;

ElemType dct[64]{ 0 };
ElemType dequant_mf[6][64]{ 0 };
ElemType res[64]{ 0 };
ElemType i_qp = rand() % 73;

void Initial()
{
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 64; ++j)
      dequant_mf[i][j] = rand();
  }

  for (int i = 0; i < 64; ++i)
  {
    dct[i] = rand();
  }
}


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

int main()
{
  Initial();
  DE_QUAN_SCALAR{}(dct, dequant_mf, i_qp, res);
  return 0;
}
