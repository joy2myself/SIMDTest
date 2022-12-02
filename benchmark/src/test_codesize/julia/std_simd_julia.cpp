#include "../../../include/core/std_simd_core.h"

using ElemType = float;

///////////////////////parameters initialization////////////////////////

ElemType xmin = -1.6, xmax = 1.6;
ElemType ymin = -1.6, ymax = 1.6;

size_t nx = 1024, ny = 1024, max_iter = 500;
ElemType cr = -0.123, ci = 0.754;

unsigned char *image = new unsigned char[nx * ny];

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

int main()
{
  JULIA_SIMD<std_simd_t_v_native<ElemType>, 
             std_simd_t_m_native<ElemType>, 
             ElemType>{}(xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
  return 0;
}
