#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"
#include "gperftools/src/gperftools/profiler.h"
#include <array>

using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t ITERATION = 5;
struct Color
{
  uint8_t alpha;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};
Color COLORS[] = {
  { 0, 0, 0, 0 },     // black
  { 0, 255, 255, 0 }, // yellow
  { 0, 255, 0, 0 },   // red
  { 0, 0, 255, 0 },   // green
  { 0, 0, 0, 255 }    // blue
};
const float xmin = -2, xmax = 2;
const float ymin = -2, ymax = 2;
const size_t nx = 512, ny = 512, max_iter = 10000;
Color *image = new Color[nx * ny];

/////////////////////////versiones of scalar///////////////////////////

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

///////////////////////versiones of simd///////////////////////////////

template<typename Vec, typename Mask, typename Tp> struct NEWTON_SIMD
{
  Mask is_equal(
      Vec re1,
      Vec im1,
      Vec re2,
      Vec im2)
  {
    const Vec tol2 = details::BroadCast<Vec, Tp>(Tp(0.0001));
    return ((re1 - re2) * (re1 - re2) + (im1 - im2) * (im1 - im2)) < tol2;
  }

  Mask converged(
      Vec re,
      Vec im,
      Vec iters,
      Vec &color_index,
      Vec &alphas)
  {
    alphas = iters * details::BroadCast<Vec, Tp>(Tp(10));
    alphas = details::Select<Mask, Vec, Tp>(alphas > details::BroadCast<Vec, Tp>(Tp(100)), details::BroadCast<Vec, Tp>(Tp(100)), alphas);

    Mask m0 = is_equal(re, im, details::BroadCast<Vec, Tp>(Tp(1.f)), details::BroadCast<Vec, Tp>(Tp(0.f)));
    color_index = details::Select<Mask, Vec, Tp>(m0, details::BroadCast<Vec, Tp>(Tp(1)), color_index);

    Mask m1 = is_equal(re, im, details::BroadCast<Vec, Tp>(Tp(-1.f)), details::BroadCast<Vec, Tp>(Tp(0.f)));
    color_index = details::Select<Mask, Vec, Tp>(m1, details::BroadCast<Vec, Tp>(Tp(2)), color_index);

    Mask m2 = is_equal(re, im, details::BroadCast<Vec, Tp>(Tp(0.f)), details::BroadCast<Vec, Tp>(Tp(1.f)));
    color_index = details::Select<Mask, Vec, Tp>(m2, details::BroadCast<Vec, Tp>(Tp(3)), color_index);

    Mask m3 = is_equal(re, im, details::BroadCast<Vec, Tp>(Tp(0.f)), details::BroadCast<Vec, Tp>(Tp(-1.f)));
    color_index = details::Select<Mask, Vec, Tp>(m3, details::BroadCast<Vec, Tp>(Tp(4)), color_index);

    return details::Or<Tp>(details::Or<Tp>(details::Or<Tp>(m0, m1), m2), m3);

  }

  void newton(
      Vec xmin,
      Vec xmax,
      size_t nx,
      Vec ymin,
      Vec ymax,
      size_t ny,
      size_t max_iter,
      Color *image)
  {
    std::size_t len = details::Len<Vec, Tp>();

    Tp index[len]{ 0 };
    for (size_t i = 0; i < len; ++i)
      index[i] = i;
    Vec iota;

    details::Store_Aligned(iota, index);

    Vec dx = (xmax - xmin) / details::BroadCast<Vec, Tp>(Tp(nx));
    Vec dy = (ymax - ymin) / details::BroadCast<Vec, Tp>(Tp(ny)), dyv = iota * dy;

    for (size_t i = 0; i < nx; ++i)
    {
      for (size_t j = 0; j < ny; j += len)
      {
        Vec re = xmin + details::BroadCast<Vec, Tp>(Tp(i)) * dx, x = re;
        Vec im = ymin + details::BroadCast<Vec, Tp>(Tp(j)) * dy + dyv, y = im;

        Vec kv = details::BroadCast<Vec, Tp>(Tp(0.f));
        Vec color_index = details::BroadCast<Vec, Tp>(Tp(0.f));
        Vec alphas = details::BroadCast<Vec, Tp>(Tp(0.f));
        Mask m = details::BroadCast<Mask, Tp>(false);

        for (size_t k = 0; !details::All<Tp>(m) && (k < max_iter);)
        {
          Vec re2 = re * re, re3 = re2 * re, re4 = re3 * re, re5 = re4 * re, re6 = re5 * re,
                                      re7 = re6 * re;
          Vec im2 = im * im, im3 = im2 * im, im4 = im3 * im, im5 = im4 * im, im6 = im5 * im,
                                      im7 = im6 * im;
          Vec coeff = details::BroadCast<Vec, Tp>(Tp(0.25f)) / ((re2 + im2) * (re2 + im2) * (re2 + im2));

          Vec three = details::BroadCast<Vec, Tp>(Tp(3.f));
          x = three * re * im2 + re * im6 + re7 + three * re5 * im2 - re3 + three * re3 * im4;
          y = three * re2 * im + im7 + three * re2 * im5 - im3 + re6 * im + three * re4 * im3;

          re -= x * coeff;
          im -= y * coeff;
          m = converged(re, im, kv, color_index, alphas);
          Mask notm = details::Not<Tp>(m);
          kv = details::Select<Mask, Vec, Tp>(notm, details::BroadCast<Vec, Tp>(Tp(++k)), kv);
        }

        for (size_t k = 0; k < len; ++k)
        {
          uint8_t ci = details::Get<Vec, Tp>(color_index, k);
          uint8_t alpha = details::Get<Vec, Tp>(alphas, k);
          Color color = COLORS[ci];
          color.alpha = alpha;
          image[ny * i + j + k] = color;
        }
      }
    }
  }
  void operator()(Tp xmin, Tp xmax, size_t nx, Tp ymin, Tp ymax, size_t ny, size_t max_iter, Color *image)
  {
    newton(details::BroadCast<Vec, Tp>(xmin), 
           details::BroadCast<Vec, Tp>(xmax), 
           nx, details::BroadCast<Vec, Tp>(ymin), 
           details::BroadCast<Vec, Tp>(ymax), ny, 
           max_iter, image);
  }
};

///////////////////////bench function//////////////////////////////////

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]() { func(xmin, xmax, nx, ymin, ymax, ny, max_iter, image); });
}

int main()
{

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("NEWTON_TEST_NATIVE").unit("NEWTON_NATIVE").relative(true);
  b_native.performanceCounters(true);

  //   ProfilerStart("newton.prof");
  //   NEWTON_SCALAR{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<eve_t_v_native<ElemType>, eve_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<tsimd_t_v_native<ElemType>, tsimd_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<xsimd_t_v_native<ElemType>, xsimd_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<highway_t_v_native<ElemType>, highway_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<mipp_t_v_native<ElemType>, mipp_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<vcl_t_v_native<ElemType>, vcl_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  //   NEWTON_SIMD<vc_t_v_native<ElemType>, vc_t_m_native<ElemType>, ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
  // ProfilerStop();

  bench<NEWTON_SCALAR>(b_native, "scalar");
  // bench<NEWTON_SIMD<scalar_t_v<ElemType>, scalar_t_m<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<NEWTON_SIMD<array_t_v<ElemType, LEN>, array_t_m<ElemType>, ElemType>>(b_native, "array");
  bench<NEWTON_SIMD<xsimd_t_v_native<ElemType>, xsimd_t_m_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<NEWTON_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<NEWTON_SIMD<vcl_t_v_native<ElemType>, vcl_t_m_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<NEWTON_SIMD<highway_t_v_native<ElemType>, highway_t_m_native<ElemType>, ElemType>>(b_native, "highway");
  // bench<NEWTON_SIMD<nsimd_t_v_native<ElemType>, nsimd_t_m_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<NEWTON_SIMD<tsimd_t_v_native<ElemType>, tsimd_t_m_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<NEWTON_SIMD<eve_t_v_native<ElemType>, eve_t_m_native<ElemType>, ElemType>>(b_native, "eve");
  bench<NEWTON_SIMD<mipp_t_v_native<ElemType>, mipp_t_m_native<ElemType>, ElemType>>(b_native, "mipp");
  bench<NEWTON_SIMD<vc_t_v_native<ElemType>, vc_t_m_native<ElemType>, ElemType>>(b_native, "vc");
  
  delete[] image;
  return 0;
}