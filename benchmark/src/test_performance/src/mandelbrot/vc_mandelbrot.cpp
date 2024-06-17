#include "../../../../include/core/vc_core.h"
#include <nanobench.h>
#include <vector>
#include <numeric>

using ElemType = float;

const std::size_t ITERATION = 10000;

///////////////////////parameters initialization////////////////////////

template<typename Vec, typename Mask, typename Tp> struct MANDELBROT_SIMD
{
  inline Vec mandel(const Mask &_active, const Vec &c_re, const Vec &c_im, int maxIters)
  {
    Vec z_re = c_re;
    Vec z_im = c_im;
    Vec vi = details::BroadCast<Vec, Tp>(Tp(0));

    for (int i = 0; i < maxIters; ++i)
    {
      Mask active = details::And<Tp>(_active, ((z_re * z_re + z_im * z_im) <= details::BroadCast<Vec, Tp>(Tp(4.f))));
      if (details::None<Tp>(active))
      {
        break;
      }

      Vec new_re = z_re * z_re - z_im * z_im;
      Vec new_im = details::BroadCast<Vec, Tp>(Tp(2.f)) * z_re * z_im;
      z_re = c_re + new_re;
      z_im = c_im + new_im;

      vi = details::Select<Mask, Vec, Tp>(active, vi + details::BroadCast<Vec, Tp>(Tp(1)), vi);
    }
    return vi;
  }

  inline void
  mandelbrot(ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, ElemType output[])
  {
    ElemType dx = (x1 - x0) / width;
    ElemType dy = (y1 - y0) / height;

    const std::size_t len = details::Len<Vec, Tp>();

    ElemType arange[len];
    std::iota(&arange[0], &arange[len], 0.f);

    Vec programIndex;
    details::Load_Unaligned(programIndex, &arange[0]);

    for (int j = 0; j < height; j++)
    {
      for (int i = 0; i < width; i += len)
      {
        Vec x =
            (details::BroadCast<Vec, Tp>(Tp(x0)) +
             (details::BroadCast<Vec, Tp>(Tp(i)) + programIndex) * details::BroadCast<Vec, Tp>(Tp(dx)));
        Vec y = details::BroadCast<Vec, Tp>(Tp((y0 + j * dy)));

        Mask active = x < details::BroadCast<Vec, Tp>(Tp(width));

        int base_index = j * width + i;
        Vec result = mandel(active, x, y, maxIters);

        Vec prev_data;
        details::Load_Unaligned(prev_data, output + base_index);
        result = details::Select<Mask, Vec, Tp>(details::Not<Tp>(active), prev_data, result);
        details::Store_Unaligned(result, output + base_index);
      }
    }
  }

  void operator()(
      ElemType x0,
      ElemType y0,
      ElemType x1,
      ElemType y1,
      int width,
      int height,
      int maxIters,
      std::vector<ElemType> _buf)
  {
    mandelbrot(x0, y0, x1, y1, width, height, maxIters, _buf.data());
  }
};

void test_vc(ankerl::nanobench::Bench &bench, ElemType x0,
              ElemType y0,
              ElemType x1,
              ElemType y1,
              int width,
              int height,
              int maxIters,
              std::vector<ElemType> _buf)
{
  MANDELBROT_SIMD<vc_t_v_native<ElemType>, vc_t_m_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("vc", [&]() {
    func(x0, y0, x1, y1, width, height, maxIters, _buf);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
}