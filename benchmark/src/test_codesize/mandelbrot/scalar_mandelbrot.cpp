#include <cstdlib>
#include <vector>
#include <numeric>

using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t LEN = 4;
const std::size_t ITERATION = 5;

const unsigned int _width = 1024;
const unsigned int _height = 768;
const ElemType x_0 = -2;
const ElemType x_1 = 1;
const ElemType y_0 = -1;
const ElemType y_1 = 1;
const int _maxIters = 256;

std::vector<ElemType> _buf(_width *_height);

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

int main()
{
  MANDELBROT_SCALAR{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
  return 0;
}
