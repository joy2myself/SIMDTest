#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"

using ElemType = float;

///////////////////////parameters initialization////////////////////////

ElemType xmin = -1.6, xmax = 1.6;
ElemType ymin = -1.6, ymax = 1.6;

size_t nx = 1024, ny = 1024, max_iter = 500;
ElemType cr = -0.123, ci = 0.754;

unsigned char *image = new unsigned char[nx * ny];

/////////////////////////versiones of scalar///////////////////////////

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

///////////////////////versiones of simd///////////////////////////////

template<typename Vec, typename Mask, typename Tp> struct JULIA_SIMD
{
  void julia(Tp xmin, Tp xmax, size_t nx, Tp ymin, Tp ymax, size_t ny, size_t max_iter, unsigned char *image, Tp real, Tp im)
  {
    std::size_t len = details::Len<Vec, Tp>();

    Tp index[len]{ 0 };
    for (size_t i = 0; i < len; ++i)
      index[i] = i;
    Vec iota;

    details::Store_Aligned(iota, index);

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
        Mask m = details::BroadCast<Mask, Tp>(true);

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

//////////////bench function///////////////////////////////////////////

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(1).run(name, [&]() { func(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci); });
}

int main()
{

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("JULIA_TEST_NATIVE").unit("JULIA_NATIVE").relative(true);
  b_native.performanceCounters(true);

  bench<JULIA_SCALAR>(b_native, "scalar");
  bench<JULIA_SIMD<scalar_t_v<ElemType>, scalar_t_m<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<JULIA_SIMD<array_t_v<ElemType, LEN>, array_t_m<ElemType>, ElemType>>(b_native, "array");
  bench<JULIA_SIMD<xsimd_t_v_native<ElemType>, xsimd_t_m_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<JULIA_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<JULIA_SIMD<vcl_t_v_native<ElemType>, vcl_t_m_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<JULIA_SIMD<highway_t_v_native<ElemType>, highway_t_m_native<ElemType>, ElemType>>(b_native, "highway");
  // bench<JULIA_SIMD<nsimd_t_v_native<ElemType>, nsimd_t_m_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<JULIA_SIMD<tsimd_t_v_native<ElemType>, tsimd_t_m_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<JULIA_SIMD<eve_t_v_native<ElemType>, eve_t_m_native<ElemType>, ElemType>>(b_native, "eve");
  bench<JULIA_SIMD<mipp_t_v_native<ElemType>, mipp_t_m_native<ElemType>, ElemType>>(b_native, "mipp");
  // bench<JULIA_SIMD<vc_t_v_native<ElemType>, vc_t_m_native<ElemType>, ElemType>>(b_native, "vc");

  return 0;
}
