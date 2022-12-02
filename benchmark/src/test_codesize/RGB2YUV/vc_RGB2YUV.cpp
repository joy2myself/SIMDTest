#include "../../../include/core/vc_core.h"

using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t ARRLENGTH = 512;
const std::size_t LEN = 4;
const std::size_t ITERATION = 5000000;

ElemType r[ARRLENGTH] = { 0 };
ElemType g[ARRLENGTH] = { 0 };
ElemType b[ARRLENGTH] = { 0 };
ElemType ya[ARRLENGTH] = { 0 };
ElemType ua[ARRLENGTH] = { 0 };
ElemType va[ARRLENGTH] = { 0 };

void Initial()
{
  for (int i = 0; i < ARRLENGTH; i++)
  {
    r[i] = random() % 256;
    g[i] = random() % 256;
    b[i] = random() % 256;
  }
}

template<typename Vec, typename Tp> struct RGB2YUV_SIMD
{
  void rgb2yuv(Tp *ra, Tp *ga, Tp *ba, Tp *ya, Tp *ua, Tp *va)
  {
    Vec a, b, c, y, u, v;
    auto len = details::Len<Vec, Tp>();
    for (int i = 0; i < ARRLENGTH; i += len)
    {
      details::Load_Aligned(a, &ra[i]);
      details::Load_Aligned(b, &ga[i]);
      details::Load_Aligned(c, &ba[i]);
      y = details::BroadCast<Vec, Tp>(Tp(0.299)) * a + details::BroadCast<Vec, Tp>(Tp(0.584)) * b +
          details::BroadCast<Vec, Tp>(Tp(0.114)) * c;
      u = details::BroadCast<Vec, Tp>(Tp(-0.14713)) * a - details::BroadCast<Vec, Tp>(Tp(0.28886)) * b +
          details::BroadCast<Vec, Tp>(Tp(0.436)) * c;
      v = details::BroadCast<Vec, Tp>(Tp(0.615)) * a - details::BroadCast<Vec, Tp>(Tp(0.51499)) * b -
          details::BroadCast<Vec, Tp>(Tp(0.10001)) * c;
      details::Store_Aligned(y, &ya[i]);
      details::Store_Aligned(u, &ua[i]);
      details::Store_Aligned(v, &va[i]);
    }
  }

  void operator()(Tp *ra, Tp *ga, Tp *ba, Tp *ya, Tp *ua, Tp *va)
  {
    rgb2yuv(ra, ga, ba, ya, ua, va);
  }
};


int main(int argc, char *argv[])
{

  Initial();
  RGB2YUV_SIMD<vc_t_v_native<ElemType>, ElemType>{}(r, g, b, ya, ua, va);
  return 0;
}