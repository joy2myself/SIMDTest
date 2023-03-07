#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench/src/include/nanobench.h"

#include <cmath>
#include <limits>
#include <random>
#include "../../all.h"

using ElemType = float;

static constexpr size_t kN = (1024 * 1024);
const std::size_t ITERATION = 10;

ElemType *a = new ElemType[kN];
ElemType *b = new ElemType[kN];
ElemType *c = new ElemType[kN];
ElemType *x1 = new ElemType[kN];
ElemType *x2 = new ElemType[kN];
ElemType *roots = new ElemType[kN];

// naive scalar code

void Initial()
{
  srand((unsigned)time(NULL));
  std::random_device rng;
  std::default_random_engine g(rng());
  std::uniform_real_distribution<ElemType> dist(-10.0f, 10.0f);

  for (size_t i = 0; i < kN; i++)
  {
    a[i] = dist(g);
    b[i] = dist(g);
    c[i] = 5.0f * dist(g);
    x1[i] = 0.0f;
    x2[i] = 0.0f;
    roots[i] = 0;
  }
}

struct QUADRATIC_SCALAR
{
  int QuadSolve(ElemType a, ElemType b, ElemType c, ElemType &x1, ElemType &x2)
  {
    ElemType delta = b * b - 4.0 * a * c;

    if (delta < 0.0)
      return 0;

    if (delta < std::numeric_limits<ElemType>::epsilon())
    {
      x1 = x2 = -0.5 * b / a;
      return 1;
    }

    if (b >= 0.0)
    {
      x1 = -0.5 * (b + sqrt(delta)) / a;
      x2 = c / (a * x1);
    }
    else
    {
      x2 = -0.5 * (b - sqrt(delta)) / a;
      x1 = c / (a * x2);
    }

    return 2;
  }
  void TestQuadSolve(const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
  {
    for (size_t i = 0; i < kN; i++)
      roots[i] = QuadSolve(a[i], b[i], c[i], x1[i], x2[i]);
  }
  void operator()(const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
  {
    TestQuadSolve(a, b, c, x1, x2, roots);
  }
};

// optimized scalar code

struct QUADRATIC_SCALAR_OPTIMIZE
{
#ifdef OpenAutoOptimize
#pragma GCC push_options
#pragma GCC optimize("O2,tree-vectorize")

  void QuadSolveOptimized(const ElemType &a, const ElemType &b, const ElemType &c, ElemType &x1, ElemType &x2, ElemType &roots)
  {
    ElemType a_inv = ElemType(1.0) / a;
    ElemType delta = b * b - ElemType(4.0) * a * c;
    ElemType s = (b >= 0) ? ElemType(1.0) : ElemType(-1.0);

    roots = delta > numeric_limits<ElemType>::epsilon() ? 2 : delta < ElemType(0.0) ? 0 : 1;

    switch (roots)
    {
    case 2:
      x1 = ElemType(-0.5) * (b + s * sqrt(delta));
      x2 = c / x1;
      x1 *= a_inv;
      return;

    case 0: return;

    case 1: x1 = x2 = ElemType(-0.5) * b * a_inv; return;

    default: return;
    }
  }
  void TestQuadSolve(const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
  {
    for (size_t i = 0; i < kN; i++)
      QuadSolveOptimized(a[i], b[i], c[i], x1[i], x2[i], roots[i]);
  }
  void operator()(const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
  {
    TestQuadSolve(a, b, c, x1, x2, roots);
  }
#pragma GCC pop_options
#endif
};

template<typename Vec, typename Mask, typename Tp> struct QUADRATIC_SIMD
{
  void QuadSolveSIMD(Vec const &a, Vec const &b, Vec const &c, Vec &x1, Vec &x2, Vec &roots)
  {
    Vec a_inv = details::BroadCast<Vec, Tp>(Tp(1.0f)) / a;
    Vec delta = b * b - details::BroadCast<Vec, Tp>(Tp(4.0f)) * a * c;
    Vec sign;
    sign = details::Select<Mask, Vec, Tp>(b >= details::BroadCast<Vec, Tp>(Tp(0.0f)), details::BroadCast<Vec, Tp>(Tp(1.0f)), sign);
    sign = details::Select<Mask, Vec, Tp>(b < details::BroadCast<Vec, Tp>(Tp(0.0f)), details::BroadCast<Vec, Tp>(Tp(-1.0f)), sign);

    auto mask0 = delta < details::BroadCast<Vec, Tp>(Tp(0.0f));
    auto mask2 = delta >= details::BroadCast<Vec, Tp>(Tp(std::numeric_limits<Tp>::epsilon()));

    Vec root1 = details::BroadCast<Vec, Tp>(Tp(-0.5f)) * b + sign * details::Sqrt<Tp>(delta);
    Vec root2 = c / root1;
    root1 = root1 * a_inv;

    auto mask1 = details::Not<Tp>(details::Or<Tp>(mask2, mask0));

    x1 = details::Select<Mask, Vec, Tp>(mask2, root1, x1);
    x2 = details::Select<Mask, Vec, Tp>(mask2, root2, x2);
    roots = details::Select<Mask, Vec, Tp>(mask2, details::BroadCast<Vec, Tp>(Tp(2)), x2);
    roots = details::Select<Mask, Vec, Tp>(details::Not<Tp>(mask2), details::BroadCast<Vec, Tp>(Tp(0)), x2);
    if (details::None<Tp>(mask1)) 
      return;

    root1 = details::BroadCast<Vec, Tp>(Tp(-0.5f)) * b * a_inv;

    roots = details::Select<Mask, Vec, Tp>(mask1, details::BroadCast<Vec, Tp>(Tp(1)), roots);
    x1 = details::Select<Mask, Vec, Tp>(mask1, root1, x1);
    x2 = details::Select<Mask, Vec, Tp>(mask1, root1, x2);
  }

  void TestQuadSolve(const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
  {
    for (size_t i = 0; i < kN; i += details::Len<Vec, Tp>())
    {
      
      Vec a_v, b_v, c_v, x1_v, x2_v, roots_v;
      details::Load_Aligned(a_v, &a[i]);
      details::Load_Aligned(b_v, &b[i]);
      details::Load_Aligned(c_v, &c[i]);
      details::Load_Aligned(x1_v, &x1[i]);
      details::Load_Aligned(x2_v, &x2[i]);
      details::Load_Aligned(roots_v, &roots[i]);

      QuadSolveSIMD(
      a_v, 
      b_v, 
      c_v, 
      x1_v, 
      x2_v, 
      roots_v
      );

      details::Store_Aligned(roots_v, &roots[i]);
    }
  }

  void operator()(const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
  {
    TestQuadSolve(a, b, c, x1, x2, roots);
  }
};

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(ITERATION).run(name, [&]() { func(a, b, c, x1, x2, roots); ankerl::nanobench::doNotOptimizeAway(func);});
}

int main()
{

  Initial();

  ////////////////////////////////NATIVE////////////////////////////

  ankerl::nanobench::Bench b_native;
  b_native.title("QUADRATIC_TEST_NATIVE").unit("QUADRATIC_NATIVE").relative(true);
  b_native.performanceCounters(true);

  bench<QUADRATIC_SCALAR>(b_native, "scalar");
  bench<QUADRATIC_SIMD<scalar_t_v<ElemType>, scalar_t_m<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<QUADRATIC_SIMD<array_t_v<ElemType, LEN>, array_t_m<ElemType>, ElemType>>(b_native, "array");
  bench<QUADRATIC_SIMD<xsimd_t_v_native<ElemType>, xsimd_t_m_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<QUADRATIC_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType>>(b_native, "std_simd");
  bench<QUADRATIC_SIMD<vcl_t_v_native<ElemType>, vcl_t_m_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<QUADRATIC_SIMD<highway_t_v_native<ElemType>, highway_t_m_native<ElemType>, ElemType>>(b_native, "highway");
  bench<QUADRATIC_SIMD<nsimd_t_v_native<ElemType>, nsimd_t_m_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<QUADRATIC_SIMD<tsimd_t_v_native<ElemType>, tsimd_t_m_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<QUADRATIC_SIMD<eve_t_v_native<ElemType>, eve_t_m_native<ElemType>, ElemType>>(b_native, "eve");
  bench<QUADRATIC_SIMD<mipp_t_v_native<ElemType>, mipp_t_m_native<ElemType>, ElemType>>(b_native, "mipp");
  // bench<QUADRATIC_SIMD<vc_t_v_native<ElemType>, vc_t_m_native<ElemType>, ElemType>>(b_native, "vc");

  delete[] a;
  delete[] b;
  delete[] c;
  delete[] x1;
  delete[] x2;
  delete[] roots;
  return 0;
}