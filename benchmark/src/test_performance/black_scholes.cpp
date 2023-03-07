#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"
#include "gperftools/src/gperftools/profiler.h"

using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t ARRLENGTH = 128*1024;
const std::size_t ITERATION = 50;

ElemType *S = new ElemType[ARRLENGTH];
ElemType *X = new ElemType[ARRLENGTH];
ElemType *T = new ElemType[ARRLENGTH];
ElemType *r = new ElemType[ARRLENGTH];
ElemType *v = new ElemType[ARRLENGTH];
ElemType *result = new ElemType[ARRLENGTH];

void Initial()
{
  for (int i = 0; i < ARRLENGTH; ++i)
  {
    S[i] = 100; // stock price
    X[i] = 98;  // option strike price
    T[i] = 2;   // time (years)
    r[i] = .02; // risk-free interest rate
    v[i] = 5;   // volatility
  }
}

/////////////////////////versiones of scalar///////////////////////////

struct BLACK_SCHOLES_SCALAR
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")
  ElemType CND(ElemType &X)
  {
    ElemType L = fabsf(X);

    ElemType k = 1.f / (1.f + 0.2316419f * L);
    ElemType k2 = k * k;
    ElemType k3 = k2 * k;
    ElemType k4 = k2 * k2;
    ElemType k5 = k3 * k2;

    const ElemType invSqrt2Pi = 0.39894228040f;
    ElemType w = (0.31938153f * k - 0.356563782f * k2 + 1.781477937f * k3 + -1.821255978f * k4 + 1.330274429f * k5);
    w *= invSqrt2Pi * expf(-L * L * .5f);

    if (X > 0.f)
      w = 1.f - w;
    return w;
  }

  void black_scholes_serial(
      ElemType Sa[],
      ElemType Xa[],
      ElemType Ta[],
      ElemType ra[],
      ElemType va[],
      ElemType result[],
      const int &count)
  {
    for (int i = 0; i < count; ++i)
    {
      ElemType S = Sa[i], X = Xa[i];
      ElemType T = Ta[i], r = ra[i];
      ElemType v = va[i];

      ElemType d1 = (logf(S / X) + (r + v * v * .5f) * T) / (v * sqrtf(T));
      ElemType d2 = d1 - v * sqrtf(T);

      result[i] = S * CND(d1) - X * expf(-r * T) * CND(d2);
    }
  }

  void
  operator()(ElemType Sa[], ElemType Xa[], ElemType Ta[], ElemType ra[], ElemType va[], ElemType result[], const int &count)
  {
    black_scholes_serial(Sa, Xa, Ta, ra, va, result, count);
  }
#pragma GCC pop_options
#else
  ElemType CND(ElemType &X)
  {
    ElemType L = fabsf(X);

    ElemType k = 1.f / (1.f + 0.2316419f * L);
    ElemType k2 = k * k;
    ElemType k3 = k2 * k;
    ElemType k4 = k2 * k2;
    ElemType k5 = k3 * k2;

    const ElemType invSqrt2Pi = 0.39894228040f;
    ElemType w = (0.31938153f * k - 0.356563782f * k2 + 1.781477937f * k3 + -1.821255978f * k4 + 1.330274429f * k5);
    w *= invSqrt2Pi * expf(-L * L * .5f);

    if (X > 0.f)
      w = 1.f - w;
    return w;
  }

  void black_scholes_serial(
      ElemType Sa[],
      ElemType Xa[],
      ElemType Ta[],
      ElemType ra[],
      ElemType va[],
      ElemType result[],
      const int &count)
  {
    for (int i = 0; i < count; ++i)
    {
      ElemType S = Sa[i], X = Xa[i];
      ElemType T = Ta[i], r = ra[i];
      ElemType v = va[i];

      ElemType d1 = (logf(S / X) + (r + v * v * .5f) * T) / (v * sqrtf(T));
      ElemType d2 = d1 - v * sqrtf(T);

      result[i] = S * CND(d1) - X * expf(-r * T) * CND(d2);
    }
  }

  void
  operator()(ElemType Sa[], ElemType Xa[], ElemType Ta[], ElemType ra[], ElemType va[], ElemType result[], const int &count)
  {
    black_scholes_serial(Sa, Xa, Ta, ra, va, result, count);
  }
#endif
};

///////////////////////versiones of simd///////////////////////////////

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

  void black_scholes_serial(float *Sa, float *Xa, float *Ta, float *ra, float *va, float *result, const int &count)
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

  void operator()(float *Sa, float *Xa, float *Ta, float *ra, float *va, float *result, const int &count)
  {
    black_scholes_serial(Sa, Xa, Ta, ra, va, result, count);
  }
};

///////////////////////bench function//////////////////////////////////

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]() { func(S, X, T, r, v, result, ARRLENGTH); });
}

int main()
{

  Initial();

  ////////////////////////////////NATIVE////////////////////////////

  // ProfilerStart("blackscholes.prof");
  // // BLACK_SCHOLES_SCALAR{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<scalar_t_v<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<xsimd_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // // BLACK_SCHOLES_SIMD<nsimd_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<tsimd_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<vcl_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<std_simd_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<mipp_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<eve_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<highway_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // BLACK_SCHOLES_SIMD<vc_t_v_native<ElemType>, ElemType>{}(S, X, T, r, v, result, ARRLENGTH);
  // ProfilerStop();


  ankerl::nanobench::Bench b_native;
  b_native.title("BLACK_SCHOLES_TEST_NATIVE").unit("BLACK_SCHOLES_NATIVE").relative(true);
  b_native.performanceCounters(true);

  bench<BLACK_SCHOLES_SCALAR>(b_native, "scalar");
  // bench<BLACK_SCHOLES_SIMD<scalar_t_v<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<BLACK_SCHOLES_SIMD<array_t_v<ElemType, LEN>, ElemType>>(b_native, "array");
  bench<BLACK_SCHOLES_SIMD<xsimd_t_v_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<BLACK_SCHOLES_SIMD<std_simd_t_v_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<BLACK_SCHOLES_SIMD<vcl_t_v_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<BLACK_SCHOLES_SIMD<highway_t_v_native<ElemType>, ElemType>>(b_native, "highway");
  // bench<BLACK_SCHOLES_SIMD<nsimd_t_v_native<ElemType>, ElemType>>(b_native, "nsimd"); // TODO: nsimd ld error for sleef
  bench<BLACK_SCHOLES_SIMD<tsimd_t_v_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<BLACK_SCHOLES_SIMD<eve_t_v_native<ElemType>, ElemType>>(b_native, "eve");
  bench<BLACK_SCHOLES_SIMD<mipp_t_v_native<ElemType>, ElemType>>(b_native, "mipp");
  bench<BLACK_SCHOLES_SIMD<vc_t_v_native<ElemType>, ElemType>>(b_native, "vc");
  return 0;
}
