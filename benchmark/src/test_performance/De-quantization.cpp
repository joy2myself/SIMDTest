#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"

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

/////////////////////////versiones of scalar///////////////////////////

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

///////////////////////versiones of simd///////////////////////////////

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

//////////////bench function///////////////////////////////////////////

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]() { func(dct, dequant_mf, i_qp, res); });
}

int main()
{

  Initial();

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("DE_QUAN_TEST_NATIVE").unit("DE_QUAN_NATIVE").relative(true);
  b_native.performanceCounters(true);

  bench<DE_QUAN_SCALAR>(b_native, "scalar");
  bench<DE_QUAN_SIMD<scalar_t_v<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<DE_QUAN_SIMD<array_t_v<ElemType, LEN>, ElemType>>(b_native, "array");
  bench<DE_QUAN_SIMD<xsimd_t_v_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<DE_QUAN_SIMD<std_simd_t_v_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<DE_QUAN_SIMD<vcl_t_v_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<DE_QUAN_SIMD<highway_t_v_native<ElemType>, ElemType>>(b_native, "highway");
  bench<DE_QUAN_SIMD<nsimd_t_v_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<DE_QUAN_SIMD<tsimd_t_v_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<DE_QUAN_SIMD<eve_t_v_native<ElemType>, ElemType>>(b_native, "eve");
  bench<DE_QUAN_SIMD<mipp_t_v_native<ElemType>, ElemType>>(b_native, "mipp");  //TODO FIX
  // bench<DE_QUAN_SIMD<vc_t_v_native<ElemType>, ElemType>>(b_native, "vc");
  
  return 0;
}
