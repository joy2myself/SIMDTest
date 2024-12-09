#include "../../../../include/core/std_simd_core.h"
#include <nanobench.h>
#include <vector>
using ElemType = float;

///////////////////////parameters initialization////////////////////////

template<typename Vec, typename Mask, typename Tp> struct JULIA_SIMD
{
  void julia(Tp xmin, Tp xmax, size_t nx, Tp ymin, Tp ymax, size_t ny, size_t max_iter, unsigned char *image, Tp real, Tp im)
  {
    std::size_t len = details::Len<Vec, Tp>();

    std::vector<Tp> index(len, 0);
    for (size_t i = 0; i < len; ++i)
      index[i] = i;
    Vec iota;

    details::Load_Unaligned(iota, index.data());

    Vec dx = details::BroadCast<Vec, Tp>(Tp((xmax - xmin) / nx));
    Vec dy = details::BroadCast<Vec, Tp>(Tp((ymax - ymin) / ny));
    Vec dyv = iota * dy;

    for (int i = 0; i < nx; ++i)
    {
      for (int j = 0; j < ny; j += len)
      {
        int k = 0;
        Vec x = details::BroadCast<Vec, Tp>(Tp(xmin)) + details::BroadCast<Vec, Tp>(Tp(i)) * dx;
        Vec cr = details::BroadCast<Vec, Tp>(Tp(real));
        Vec zr = x;
        Vec y = details::BroadCast<Vec, Tp>(Tp(ymin)) + details::BroadCast<Vec, Tp>(Tp(j)) * dy + dyv;
        Vec ci = details::BroadCast<Vec, Tp>(Tp(im)); 
        Vec zi = y;

        Vec kv = details::BroadCast<Vec, Tp>(Tp(0));
        Mask m = (kv == kv);

        do
        {
          x = zr * zr - zi * zi + cr;
          y = details::BroadCast<Vec, Tp>(Tp(2.f)) * zr * zi + ci;

          zr = details::Select<Mask, Vec, Tp>(m, x, zr);
          zi = details::Select<Mask, Vec, Tp>(m, y, zi);
          kv = details::Select<Mask, Vec, Tp>(m, details::BroadCast<Vec, Tp>(Tp(++k)), kv);

          m = zr * zr + zi * zi < details::BroadCast<Vec, Tp>(Tp(4.f));
        } while (k < max_iter && details::All<Tp>(m));

        for (size_t k = 0; k < len; ++k)
          image[ny * i + j + k] = (unsigned char) details::Get<Vec, Tp>(kv, k);
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