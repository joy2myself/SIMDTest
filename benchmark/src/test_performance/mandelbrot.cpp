#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"
#include "gperftools/src/gperftools/profiler.h"

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
const int _maxIters = 128;

std::vector<ElemType> _buf(_width *_height);

/////////////////////////versiones of scalar///////////////////////////

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

///////////////////////versiones of simd///////////////////////////////

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
    details::Load_Aligned(programIndex, &arange[0]);

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

///////////////////////bench function//////////////////////////////////

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]() { func(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf); });
}

int main()
{

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("MANDELBROT_TEST_NATIVE").unit("MANDELBROT_NATIVE").relative(true);
  b_native.performanceCounters(true);


//   ProfilerStart("mandel.prof");
//   MANDELBROT_SCALAR{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<scalar_t_v<ElemType>, scalar_t_m<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<xsimd_t_v_native<ElemType>, xsimd_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//  // MANDELBROT_SIMD<nsimd_t_v_native<ElemType>, nsimd_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<tsimd_t_v_native<ElemType>, tsimd_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<vcl_t_v_native<ElemType>, vcl_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<mipp_t_v_native<ElemType>, mipp_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<eve_t_v_native<ElemType>, eve_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   MANDELBROT_SIMD<highway_t_v_native<ElemType>, highway_t_m_native<ElemType>, ElemType>{}(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
//   ProfilerStop();



  bench<MANDELBROT_SCALAR>(b_native, "scalar");
  // bench<MANDELBROT_SIMD<scalar_t_v<ElemType>, scalar_t_m<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<MANDELBROT_SIMD<xsimd_t_v_native<ElemType>, xsimd_t_m_native<ElemType>, ElemType>>(b_native, "xsimd");
  // bench<MANDELBROT_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType>>(b_native, "std_simd");
  // bench<MANDELBROT_SIMD<vcl_t_v_native<ElemType>, vcl_t_m_native<ElemType>, ElemType>>(b_native, "vcl");
  // bench<MANDELBROT_SIMD<highway_t_v_native<ElemType>, highway_t_m_native<ElemType>, ElemType>>(b_native, "highway");
  bench<MANDELBROT_SIMD<nsimd_t_v_native<ElemType>, nsimd_t_m_native<ElemType>, ElemType>>(b_native, "nsimd");
  // bench<MANDELBROT_SIMD<tsimd_t_v_native<ElemType>, tsimd_t_m_native<ElemType>, ElemType>>(b_native, "tsimd");
  // bench<MANDELBROT_SIMD<eve_t_v_native<ElemType>, eve_t_m_native<ElemType>, ElemType>>(b_native, "eve");
  // bench<MANDELBROT_SIMD<mipp_t_v_native<ElemType>, mipp_t_m_native<ElemType>, ElemType>>(b_native, "mipp");
  // bench<MANDELBROT_SIMD<vc_t_v_native<ElemType>, vc_t_m_native<ElemType>, ElemType>>(b_native, "vc");
  // return 0;
}
