#include <cstdlib>
#include <cmath>
using ElemType = float;

///////////////////////parameters initialization////////////////////////

const std::size_t ARRLENGTH = 128 * 1024;
const std::size_t LEN = 4;
const std::size_t ITERATION = 5;

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


int main()
{
  Initial();
  BLACK_SCHOLES_SCALAR{}(S, X, T, r, v, result, ARRLENGTH);
  return 0;
}