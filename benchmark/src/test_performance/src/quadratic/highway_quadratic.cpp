#include "../../../../include/core/highway_core.h"
#include <nanobench.h>
#include <cmath>
#include <limits>
#include <random>

using ElemType = float;

const size_t kN = (1024 * 1024);
const std::size_t ITERATION = 100;

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
      details::Load_Unaligned(a_v, &a[i]);
      details::Load_Unaligned(b_v, &b[i]);
      details::Load_Unaligned(c_v, &c[i]);
      details::Load_Unaligned(x1_v, &x1[i]);
      details::Load_Unaligned(x2_v, &x2[i]);
      details::Load_Unaligned(roots_v, &roots[i]);

      QuadSolveSIMD(
      a_v, 
      b_v, 
      c_v, 
      x1_v, 
      x2_v, 
      roots_v
      );

      details::Store_Unaligned(roots_v, &roots[i]);
    }
  }

  void operator()(const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
  {
    TestQuadSolve(a, b, c, x1, x2, roots);
  }
};


void test_highway(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
{
  QUADRATIC_SIMD<highway_t_v_native<ElemType>, highway_t_m_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("highway", [&]() {
    func(a, b, c, x1, x2, roots);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
}