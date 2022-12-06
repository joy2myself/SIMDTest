#include <cstdlib>
#include <nanobench.h>
using ElemType = float;

ElemType xmin = -1.6, xmax = 1.6;
ElemType ymin = -1.6, ymax = 1.6;

size_t nx = 1024, ny = 1024, max_iter = 500;
ElemType cr = -0.123, ci = 0.754;

unsigned char *image = new unsigned char[nx * ny];

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


int main()
{
  JULIA_SCALAR{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
  return 0;
}
