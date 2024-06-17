#include <cstdlib>
#include <nanobench.h>
using ElemType = float;

const std::size_t ITERATION = 10000;

struct MANDELBROT_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  inline int mandel(ElemType c_re, ElemType c_im, int count)
  {
    ElemType z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i)
    {
      if (z_re * z_re + z_im * z_im > 4.f)
      {
        break;
      }

      ElemType new_re = z_re * z_re - z_im * z_im;
      ElemType new_im = 2.f * z_re * z_im;
      z_re = c_re + new_re;
      z_im = c_im + new_im;
    }

    return i;
  }

  void
  mandelbrot(ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIterations, ElemType output[])
  {
    ElemType dx = (x1 - x0) / width;
    ElemType dy = (y1 - y0) / height;

    for (int j = 0; j < height; j++)
    {
      for (int i = 0; i < width; ++i)
      {
        ElemType x = x0 + i * dx;
        ElemType y = y0 + j * dy;

        int index = (j * width + i);
        output[index] = mandel(x, y, maxIterations);
      }
    }
  }
#pragma GCC pop_options
#else
  inline int mandel(ElemType c_re, ElemType c_im, int count)
  {
    ElemType z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i)
    {
      if (z_re * z_re + z_im * z_im > 4.f)
      {
        break;
      }

      ElemType new_re = z_re * z_re - z_im * z_im;
      ElemType new_im = 2.f * z_re * z_im;
      z_re = c_re + new_re;
      z_im = c_im + new_im;
    }

    return i;
  }

  void
  mandelbrot(ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIterations, ElemType output[])
  {
    ElemType dx = (x1 - x0) / width;
    ElemType dy = (y1 - y0) / height;

    for (int j = 0; j < height; j++)
    {
      for (int i = 0; i < width; ++i)
      {
        ElemType x = x0 + i * dx;
        ElemType y = y0 + j * dy;

        int index = (j * width + i);
        output[index] = mandel(x, y, maxIterations);
      }
    }
  }
#endif
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

void test_scalar(ankerl::nanobench::Bench &bench, ElemType x0,
              ElemType y0,
              ElemType x1,
              ElemType y1,
              int width,
              int height,
              int maxIters,
              std::vector<ElemType> _buf)
{
  MANDELBROT_SCALAR func;
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    func(x0, y0, x1, y1, width, height, maxIters, _buf);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
}