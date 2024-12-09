#include "../../../../include/core/std_simd_core.h"
#include <nanobench.h>
#include <vector>
using ElemType = float;

///////////////////////parameters initialization////////////////////////

template<typename Vec, typename Mask, typename Tp>
struct JULIA_SIMD
{
  void julia(Tp xmin, Tp xmax, size_t nx, Tp ymin, Tp ymax, size_t ny, size_t max_iter, unsigned char *image, Tp real, Tp im)
  {
    std::size_t len = details::Len<Vec, Tp>();

    std::vector<Tp> index(len, 0);
    for (size_t idx = 0; idx < len; ++idx)
      index[idx] = idx;
    Vec iota;

    details::Load_Unaligned(iota, index.data());

    Vec dx = details::BroadCast<Vec, Tp>(Tp((xmax - xmin) / nx));
    Vec dy = details::BroadCast<Vec, Tp>(Tp((ymax - ymin) / ny));
    Vec dyv = iota * dy;

    for (size_t i = 0; i < nx; ++i)
    {
      for (size_t j = 0; j < ny; j += 2 * len)
      {
        // 块 1
        Vec x1 = details::BroadCast<Vec, Tp>(Tp(xmin)) + details::BroadCast<Vec, Tp>(Tp(i)) * dx;
        Vec zr1 = x1;
        Vec y1 = details::BroadCast<Vec, Tp>(Tp(ymin)) + details::BroadCast<Vec, Tp>(Tp(j)) * dy + dyv;
        Vec zi1 = y1;
        Vec cr1 = details::BroadCast<Vec, Tp>(Tp(real));
        Vec ci1 = details::BroadCast<Vec, Tp>(Tp(im));
        Vec kv1 = details::BroadCast<Vec, Tp>(Tp(0));
        Mask m1 = (kv1 == kv1);

        int k1 = 0;

        // 块 2
        Vec x2 = x1; // x 不变
        Vec zr2 = x2;
        Vec y2 = details::BroadCast<Vec, Tp>(Tp(ymin)) + details::BroadCast<Vec, Tp>(Tp(j + len)) * dy + dyv;
        Vec zi2 = y2;
        Vec kv2 = details::BroadCast<Vec, Tp>(Tp(0));
        Mask m2 = (kv2 == kv2);

        int k2 = 0;

        // 块 1 和块 2 Julia 集迭代
        do
        {
          // 块 1 计算
          Vec x1_next = zr1 * zr1 - zi1 * zi1 + cr1;
          Vec y1_next = details::BroadCast<Vec, Tp>(Tp(2.f)) * zr1 * zi1 + ci1;

          zr1 = details::Select<Mask, Vec, Tp>(m1, x1_next, zr1);
          zi1 = details::Select<Mask, Vec, Tp>(m1, y1_next, zi1);
          kv1 = details::Select<Mask, Vec, Tp>(m1, details::BroadCast<Vec, Tp>(Tp(++k1)), kv1);

          m1 = zr1 * zr1 + zi1 * zi1 < details::BroadCast<Vec, Tp>(Tp(4.f));

          // 块 2 计算
          Vec x2_next = zr2 * zr2 - zi2 * zi2 + cr1;
          Vec y2_next = details::BroadCast<Vec, Tp>(Tp(2.f)) * zr2 * zi2 + ci1;

          zr2 = details::Select<Mask, Vec, Tp>(m2, x2_next, zr2);
          zi2 = details::Select<Mask, Vec, Tp>(m2, y2_next, zi2);
          kv2 = details::Select<Mask, Vec, Tp>(m2, details::BroadCast<Vec, Tp>(Tp(++k2)), kv2);

          m2 = zr2 * zr2 + zi2 * zi2 < details::BroadCast<Vec, Tp>(Tp(4.f));

        } while ((k1 < max_iter || k2 < max_iter) && (details::All<Tp>(m1) || details::All<Tp>(m2)));

        // 写回结果
        for (size_t k = 0; k < len; ++k)
        {
          image[ny * i + j + k] = (unsigned char)details::Get<Vec, Tp>(kv1, k);
          image[ny * i + j + len + k] = (unsigned char)details::Get<Vec, Tp>(kv2, k);
        }
      }
    }
  }

  void
  operator()(Tp xmin, Tp xmax, size_t nx, Tp ymin, Tp ymax, size_t ny, size_t max_iter, unsigned char *image, Tp real, Tp im)
  {
    julia(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, real, im);
  }
};

// 使用 nanobench 对simd实现进行性能测试
void test_std_simd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im)
{
#if defined(USE_PLCT_SIMD)
  JULIA_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType> func;
  bench.minEpochIterations(5).run("plct_simd", [&]() {
    func(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, real, im);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#else
  JULIA_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType> func;
  // 配置 nanobench 的性能测试，指定最少迭代次数，执行simd实现并记录性能结果
  bench.minEpochIterations(5).run("std_simd", [&]() {
    func(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, real, im);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#endif
}