#include <cstdlib>
#include <nanobench.h>
using ElemType = float;

struct JULIA_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  void julia(
      ElemType xmin,
      ElemType xmax,
      size_t nx,
      ElemType ymin,
      ElemType ymax,
      size_t ny,
      size_t max_iter,
      unsigned char *image,
      ElemType real,
      ElemType im)
  {
    ElemType dx = (xmax - xmin) / ElemType(nx);
    ElemType dy = (ymax - ymin) / ElemType(ny);

    for (size_t i = 0; i < nx; ++i)
    {
      for (size_t j = 0; j < ny; ++j)
      {
        size_t k = 0;
        ElemType x = xmin + ElemType(i) * dx, cr = real, zr = x;
        ElemType y = ymin + ElemType(j) * dy, ci = im, zi = y;

        do
        {
          x = zr * zr - zi * zi + cr;
          y = ElemType(2.0) * zr * zi + ci;
          zr = x;
          zi = y;
        } while (++k < max_iter && (zr * zr + zi * zi < ElemType(4.0)));

        image[ny * i + j] = k;
      }
    }
  }
#pragma GCC pop_options
#else
  void julia(
      ElemType xmin,
      ElemType xmax,
      size_t nx,
      ElemType ymin,
      ElemType ymax,
      size_t ny,
      size_t max_iter,
      unsigned char *image,
      ElemType real,
      ElemType im)
  {
    ElemType dx = (xmax - xmin) / ElemType(nx);
    ElemType dy = (ymax - ymin) / ElemType(ny);

    for (size_t i = 0; i < nx; ++i)
    {
      for (size_t j = 0; j < ny; ++j)
      {
        size_t k = 0;
        ElemType x = xmin + ElemType(i) * dx, cr = real, zr = x;
        ElemType y = ymin + ElemType(j) * dy, ci = im, zi = y;

        do
        {
          x = zr * zr - zi * zi + cr;
          y = ElemType(2.0) * zr * zi + ci;
          zr = x;
          zi = y;
        } while (++k < max_iter && (zr * zr + zi * zi < ElemType(4.0)));

        image[ny * i + j] = k;
      }
    }
  }
#endif
  void operator()(
      ElemType xmin,
      ElemType xmax,
      size_t nx,
      ElemType ymin,
      ElemType ymax,
      size_t ny,
      size_t max_iter,
      unsigned char *image,
      ElemType real,
      ElemType im)
  {
    julia(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, real, im);
  }
};

// 使用 nanobench 对标量实现进行性能测试
void test_scalar(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im)
{ 
  JULIA_SCALAR f;
  // 配置 nanobench 的性能测试，指定最少迭代次数，执行标量实现并记录性能结果
  bench.minEpochIterations(5).run("scalar", [&]() {
    f(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, real, im);
    ankerl::nanobench::doNotOptimizeAway(f);});
}