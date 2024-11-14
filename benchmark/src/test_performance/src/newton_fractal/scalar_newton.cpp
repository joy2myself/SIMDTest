#include <cstdlib>
#include <nanobench.h>
#include <array>
using ElemType = float;

const std::size_t ITERATION = 50000;
struct Color
{
  uint8_t alpha;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};
extern Color COLORS[5];

struct NEWTON_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  bool is_equal(ElemType re1, ElemType im1, ElemType re2, ElemType im2)
  {
    const ElemType tol2 = 0.0001;
    return ((re1 - re2) * (re1 - re2) + (im1 - im2) * (im1 - im2)) < tol2;
  }

  bool converged(ElemType r_e, ElemType i_m, int iters, uint8_t &color_index, uint8_t &alpha)
  {
    alpha = std::min(iters * 10, 100);

    if (is_equal(r_e, i_m, ElemType(1), ElemType(0)))
    {
      color_index = 1;
      return true;
    }

    if (is_equal(r_e, i_m, ElemType(-1), ElemType(0)))
    {
      color_index = 2;
      return true;
    }

    if (is_equal(r_e, i_m, ElemType(0), ElemType(1)))
    {
      color_index = 3;
      return true;
    }

    if (is_equal(r_e, i_m, ElemType(0), ElemType(-1)))
    {
      color_index = 4;
      return true;
    }

    return false;
  }
  void newton(ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image)
  {
    ElemType dx = (xmax - xmin) / ElemType(nx);
    ElemType dy = (ymax - ymin) / ElemType(ny);

    for (size_t i = 0; i < nx; ++i)
    {
      for (size_t j = 0; j < ny; ++j)
      {
        uint8_t color_index = 0, alpha = 0;
        ElemType re = xmin + ElemType(i) * dx, x = re;
        ElemType im = ymin + ElemType(j) * dy, y = im;
        bool has_converged = false;

        for (size_t k = 0; !has_converged && (k < max_iter);)
        {
          ElemType re2 = re * re, re3 = re2 * re, re4 = re3 * re, re5 = re4 * re, re6 = re5 * re, re7 = re6 * re;
          ElemType im2 = im * im, im3 = im2 * im, im4 = im3 * im, im5 = im4 * im, im6 = im5 * im, im7 = im6 * im;
          ElemType coeff = ElemType(0.25) / ((re2 + im2) * (re2 + im2) * (re2 + im2));

          x = ElemType(3) * re * im2 + re * im6 + re7 + ElemType(3) * re5 * im2 - re3 + ElemType(3) * re3 * im4;
          y = ElemType(3) * re2 * im + im7 + ElemType(3) * re2 * im5 - im3 + re6 * im + ElemType(3) * re4 * im3;

          re -= x * coeff;
          im -= y * coeff;
          has_converged = converged(re, im, k, color_index, alpha);
          ++k;
        }
        Color color = COLORS[color_index];
        color.alpha = alpha;
        image[ny * i + j] = color;
      }
    }
  }
#pragma GCC pop_options
#else
  bool is_equal(ElemType re1, ElemType im1, ElemType re2, ElemType im2)
  {
    const ElemType tol2 = 0.0001;
    return ((re1 - re2) * (re1 - re2) + (im1 - im2) * (im1 - im2)) < tol2;
  }

  bool converged(ElemType r_e, ElemType i_m, int iters, uint8_t &color_index, uint8_t &alpha)
  {
    alpha = std::min(iters * 10, 100);

    if (is_equal(r_e, i_m, ElemType(1), ElemType(0)))
    {
      color_index = 1;
      return true;
    }

    if (is_equal(r_e, i_m, ElemType(-1), ElemType(0)))
    {
      color_index = 2;
      return true;
    }

    if (is_equal(r_e, i_m, ElemType(0), ElemType(1)))
    {
      color_index = 3;
      return true;
    }

    if (is_equal(r_e, i_m, ElemType(0), ElemType(-1)))
    {
      color_index = 4;
      return true;
    }

    return false;
  }
  void newton(ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image)
  {
    ElemType dx = (xmax - xmin) / ElemType(nx);
    ElemType dy = (ymax - ymin) / ElemType(ny);

    for (size_t i = 0; i < nx; ++i)
    {
      for (size_t j = 0; j < ny; ++j)
      {
        uint8_t color_index = 0, alpha = 0;
        ElemType re = xmin + ElemType(i) * dx, x = re;
        ElemType im = ymin + ElemType(j) * dy, y = im;
        bool has_converged = false;

        for (size_t k = 0; !has_converged && (k < max_iter);)
        {
          ElemType re2 = re * re, re3 = re2 * re, re4 = re3 * re, re5 = re4 * re, re6 = re5 * re, re7 = re6 * re;
          ElemType im2 = im * im, im3 = im2 * im, im4 = im3 * im, im5 = im4 * im, im6 = im5 * im, im7 = im6 * im;
          ElemType coeff = ElemType(0.25) / ((re2 + im2) * (re2 + im2) * (re2 + im2));

          x = ElemType(3) * re * im2 + re * im6 + re7 + ElemType(3) * re5 * im2 - re3 + ElemType(3) * re3 * im4;
          y = ElemType(3) * re2 * im + im7 + ElemType(3) * re2 * im5 - im3 + re6 * im + ElemType(3) * re4 * im3;

          re -= x * coeff;
          im -= y * coeff;
          has_converged = converged(re, im, k, color_index, alpha);
          ++k;
        }
        Color color = COLORS[color_index];
        color.alpha = alpha;
        image[ny * i + j] = color;
      }
    }
  }
#endif
  void operator()(ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image)
  {
    newton(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  }
};


void test_scalar(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image)
{ 
  NEWTON_SCALAR f;
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    f(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    ankerl::nanobench::doNotOptimizeAway(f);});
}