#include "../../../../include/core/tsimd_core.h"
#include <nanobench.h>
#include <vector>
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

template<typename Vec, typename Mask, typename Tp> struct NEWTON_SIMD
{
  Mask is_equal_v(Vec re1, Vec im1, Vec re2, Vec im2)
  { 
      const Vec tol2 = details::BroadCast<Vec, Tp>(0.0001f);
      return ((re1 - re2) * (re1 - re2) + (im1 - im2) * (im1 - im2)) < tol2;
  }

  Mask converged_v(Vec re, Vec im, Vec iters, Vec& color_index, Vec& alphas)
  {
      alphas = iters * details::BroadCast<Vec, Tp>(10);
      alphas = details::Select<Mask, Vec, Tp>(alphas > details::BroadCast<Vec, Tp>(100), details::BroadCast<Vec, Tp>((100)), alphas);

      Mask m0 = is_equal_v(re, im, details::BroadCast<Vec, Tp>((1.f)), details::BroadCast<Vec, Tp>((0.f)));
      color_index = details::Select<Mask, Vec, Tp>(m0, details::BroadCast<Vec, Tp>((1)), color_index);

      Mask m1 = is_equal_v(re, im, details::BroadCast<Vec, Tp>((-1.f)), details::BroadCast<Vec, Tp>((0.f)));
      color_index = details::Select<Mask, Vec, Tp>(m1, details::BroadCast<Vec, Tp>((2)), color_index);

      Mask m2 = is_equal_v(re, im, details::BroadCast<Vec, Tp>((0.f)), details::BroadCast<Vec, Tp>((1.f)));
      color_index = details::Select<Mask, Vec, Tp>(m2, details::BroadCast<Vec, Tp>((3)), color_index);

      Mask m3 = is_equal_v(re, im, details::BroadCast<Vec, Tp>((0.f)), details::BroadCast<Vec, Tp>((-1.f)));
      color_index = details::Select<Mask, Vec, Tp>(m3, details::BroadCast<Vec, Tp>((4)), color_index);

      return details::Or<Tp>(details::Or<Tp>(details::Or<Tp>(m0, m1), m2), m3);
  }

  void newton_v(Vec xmin, Vec xmax, size_t nx,
                Vec ymin, Vec ymax, size_t ny,
                size_t max_iter, Color *image)
  {   
      Color COLORS[] = {
          {0, 0, 0, 0},     // black
          {0, 255, 255, 0}, // yellow
          {0, 255, 0, 0},   // red
          {0, 0, 255, 0},   // green
          {0, 0, 0, 255}    // blue
      };

      size_t len = details::Len<Vec, Tp>();
      std::vector<Tp> index(len, 0);
      for (size_t i = 0; i < len; ++i)
          index[i] = i;
      Vec iota;
      details::Load_Unaligned<Vec, Tp>(iota, index.data());

      Vec dx = (xmax - xmin) / details::BroadCast<Vec, Tp>(nx);
      Vec dy = (ymax - ymin) / details::BroadCast<Vec, Tp>(ny), dyv = (iota * dy);
      
      for (size_t i = 0; i < nx; ++i) {
          for (size_t j = 0; j < ny; j += len) {
              Vec re = xmin + details::BroadCast<Vec, Tp>(i) * dx,       x = re;
              Vec im = ymin + details::BroadCast<Vec, Tp>(j) * dy + dyv, y = im;

              Vec kv = details::BroadCast<Vec, Tp>(ElemType(0));
              Vec color_index = details::BroadCast<Vec, Tp>(ElemType(0));
              Vec alphas = details::BroadCast<Vec, Tp>(ElemType(0));
              Mask m = details::BroadCast<Vec, Tp>(0) > details::BroadCast<Vec, Tp>(1);

              for (size_t k = 0; !details::All<Tp>(m) && (k < max_iter); ) {
                  Vec re2 = re * re, re3 = re2 * re, re4 = re3 * re, re5 = re4 * re, re6 = re5 * re, re7 = re6 * re;

                  Vec im2 = im * im, im3 = im2 * im, im4 = im3 * im, im5 = im4 * im, im6 = im5 * im, im7 = im6 * im;

                  Vec coeff = details::BroadCast<Vec, Tp>(0.25) / ((re2 + im2) * (re2 + im2) * (re2 + im2));
                  Vec three = details::BroadCast<Vec, Tp>(3.f);

                  x = three * re * im2 + re * im6 + re7 + three * re5 * im2 - re3 + three * re3 * im4;
                  y = three * re2 * im + im7 + three * re2 * im5 - im3 + re6 * im + three * re4 * im3;

                  re -= x * coeff;
                  im -= y * coeff;
                  m = converged_v(re, im, kv, color_index, alphas);
                  Mask notm = details::Not<Tp>(m);
                  kv = details::Select<Mask, Vec, Tp>(notm, details::BroadCast<Vec, Tp>(++k), kv);
              }

              for (size_t k = 0; k < len; ++k) {
                  uint8_t ci = details::Get<Vec, Tp>(color_index, k);
                  uint8_t alpha = details::Get<Vec, Tp>(alphas, k);
                  Color color = COLORS[ci];
                  color.alpha = alpha;
                  image[ny * i + j + k] = color;
              }
          }
      }
  }

  void test_newton(ElemType xmin, ElemType xmax, size_t nx,
                      ElemType ymin, ElemType ymax, size_t ny,
                      int max_iter, Color *image){
      newton_v(details::BroadCast<Vec, Tp>(xmin), 
            details::BroadCast<Vec, Tp>(xmax), 
            nx, details::BroadCast<Vec, Tp>(ymin), 
            details::BroadCast<Vec, Tp>(ymax), ny, 
            max_iter, image);
  }

};


void test_tsimd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image)
{
  NEWTON_SIMD<tsimd_t_v_native<ElemType>, tsimd_t_m_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("tsimd", [&]() {
    func.test_newton(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
}