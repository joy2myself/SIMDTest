#include "../../../include/core/std_simd_core.h"

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


template<typename Vec, typename Tp> struct DE_QUAN_SIMD
{
  void dequant_8x8(ElemType dct[64], ElemType dequant_mf[6][64], ElemType i_qp, ElemType res[64])
  {
    std::size_t len = details::Len<Vec, Tp>();
    const ElemType i_mf = i_qp % 6;
    const ElemType i_qbits = i_qp / 6 - 6;
    Vec tmp_dct;
    Vec tmp_dequant_mf;

    if (i_qbits >= 0)
    {
      for (int i = 0; i < 64; i += len)
      {
        details::Load_Aligned(tmp_dct, &dct[i]);
        details::Load_Aligned(tmp_dequant_mf, &dequant_mf[i_mf][i]);
        tmp_dct = details::LeftShift<Vec, Tp>(tmp_dct * tmp_dequant_mf, i_qbits);
        details::Store_Aligned(tmp_dct, &res[i]);
      }
    }
    else
    {
      const Vec f =
          details::LeftShift<Vec, Tp>(details::BroadCast<Vec, Tp>(Tp(1)), -i_qbits - 1);
      for (int i = 0; i < 64; i += len)
      {
        details::Load_Aligned(tmp_dct, &dct[i]);
        details::Load_Aligned(tmp_dequant_mf, &dequant_mf[i_mf][i]);
        tmp_dct = details::LeftShift<Vec, Tp>(tmp_dct * tmp_dequant_mf + f, -i_qbits);
        details::Store_Aligned(tmp_dct, &res[i]);
      }
    }
  }
  void operator()(ElemType dct[64], ElemType dequant_mf[6][64], ElemType i_qp, ElemType res[64])
  {
    dequant_8x8(dct, dequant_mf, i_qp, res);
  }
};

int main()
{
  Initial();
  DE_QUAN_SIMD<std_simd_t_v_native<ElemType>, ElemType>{}(dct, dequant_mf, i_qp, res);
  return 0;
}
