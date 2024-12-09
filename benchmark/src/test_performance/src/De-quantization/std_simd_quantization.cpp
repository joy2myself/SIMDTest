#include "../../../../include/core/std_simd_core.h"
#include <nanobench.h>

using ElemType = int32_t;

///////////////////////parameters initialization////////////////////////

#if defined(__SSE2__)
const std::size_t ARRLENGTH = 64;
#else
const std::size_t ARRLENGTH = 128;
#endif
const std::size_t ITERATION = 5000;

template<typename Vec, typename Tp> struct DE_QUAN_SIMD
{
  void dequant_8x8(ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
  {
    std::size_t len = details::Len<Vec, Tp>();
    const ElemType i_mf = i_qp % 6;
    const ElemType i_qbits = i_qp / 6 - 6;
    Vec tmp_dct;
    Vec tmp_dequant_mf;

    if (i_qbits >= 0)
    {
      for (int i = 0; i < ARRLENGTH; i += len)
      {
        details::Load_Unaligned(tmp_dct, &dct[i]);
        details::Load_Unaligned(tmp_dequant_mf, &dequant_mf[i_mf][i]);
        tmp_dct = details::LeftShift<Vec, Tp>(tmp_dct * tmp_dequant_mf, i_qbits);
        details::Store_Unaligned(tmp_dct, &res[i]);
      }
    }
    else
    {
      const Vec f =
          details::LeftShift<Vec, Tp>(details::BroadCast<Vec, Tp>(Tp(1)), -i_qbits - 1);
      for (int i = 0; i < ARRLENGTH; i += len)
      {
        details::Load_Unaligned(tmp_dct, &dct[i]);
        details::Load_Unaligned(tmp_dequant_mf, &dequant_mf[i_mf][i]);
        tmp_dct = details::LeftShift<Vec, Tp>(tmp_dct * tmp_dequant_mf + f, -i_qbits);
        details::Store_Unaligned(tmp_dct, &res[i]);
      }
    }
  }
  void operator()(ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
  {
    dequant_8x8(dct, dequant_mf, i_qp, res);
  }
};


// 使用 nanobench 对simd实现进行性能测试
void test_std_simd(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH])
{
#if defined(USE_PLCT_SIMD)
  DE_QUAN_SIMD<std_simd_t_v_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("plct_simd", [&]() {
    func(dct, dequant_mf, i_qp, res);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#else
  DE_QUAN_SIMD<std_simd_t_v_native<ElemType>, ElemType> func;
  // 配置 nanobench 的性能测试，指定最少迭代次数，执行simd实现并记录性能结果
  bench.minEpochIterations(ITERATION).run("std_simd", [&]() {
    func(dct, dequant_mf, i_qp, res);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#endif
}

