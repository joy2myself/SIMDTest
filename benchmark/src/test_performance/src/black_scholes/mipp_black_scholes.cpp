#include "../../../../include/core/mipp_core.h"
#include <nanobench.h>
using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t ARRLENGTH = 128 * 1024;
const std::size_t LEN = 4;
const std::size_t ITERATION = 5;

template<typename Vec, typename Tp> struct BLACK_SCHOLES_SIMD
{
  Vec CND(Vec &X)
  {
    Vec L = details::Fabs<Tp>(X);

    Vec k = details::BroadCast<Vec, Tp>(Tp(1.f)) /
            (details::BroadCast<Vec, Tp>(Tp(1.f)) + details::BroadCast<Vec, Tp>(Tp(0.2316419f)) * L);

    Vec k2 = k * k;
    Vec k3 = k2 * k;
    Vec k4 = k2 * k2;
    Vec k5 = k3 * k2;

    const Vec invSqrt2Pi = details::BroadCast<Vec, Tp>(Tp(0.39894228040f));
    Vec w =
        (details::BroadCast<Vec, Tp>(Tp(0.31938153f)) * k - details::BroadCast<Vec, Tp>(Tp(0.356563782f)) * k2 +
         details::BroadCast<Vec, Tp>(Tp(1.781477937f)) * k3 + details::BroadCast<Vec, Tp>(Tp(-1.821255978f)) * k4 +
         details::BroadCast<Vec, Tp>(Tp(1.330274429f)) * k5);

    w *= invSqrt2Pi * details::Exp<Tp>(details::BroadCast<Vec, Tp>(Tp(-1)) * L * L * details::BroadCast<Vec, Tp>(Tp(.5f)));

    auto active = X > details::BroadCast<Vec, Tp>(Tp(0.f));

    w = details::Select<decltype(active), Vec, Tp>(active, details::BroadCast<Vec, Tp>(Tp(1.f)) - w, w);
    return w;
  }

  void black_scholes_serial(ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count)
  {
    std::size_t len = details::Len<Vec, Tp>();
    for (std::size_t i = 0; i < ARRLENGTH; i += len)
    {
      Vec S, X, T, r, v;

      details::Load_Unaligned(S, &Sa[i]);
      details::Load_Unaligned(T, &Ta[i]);
      details::Load_Unaligned(X, &Xa[i]);
      details::Load_Unaligned(r, &ra[i]);
      details::Load_Unaligned(v, &va[i]);

      Vec d1 =
          (details::Log<Tp>(S / X) + (r + v * v * details::BroadCast<Vec, Tp>(Tp(.5f))) * T) / (v * details::Sqrt<Tp>(T));
      Vec d2 = d1 - v * details::Sqrt<Tp>(T);

      Vec res = S * CND(d1) - X * details::Exp<Tp>(details::BroadCast<Vec, Tp>(Tp(-1)) * r * T) * CND(d2);
      details::Store_Unaligned(res, &result[i]);
    }
  }

  void operator()(ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count)
  {
    black_scholes_serial(Sa, Xa, Ta, ra, va, result, count);
  }
};


void test_mipp(ankerl::nanobench::Bench &bench, ElemType *Sa, ElemType *Xa, ElemType *Ta, ElemType *ra, ElemType *va, ElemType *result, const int &count)
{
  BLACK_SCHOLES_SIMD<mipp_t_v_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("mipp", [&]() {
    func(Sa, Xa, Ta, ra, va, result, count);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
}