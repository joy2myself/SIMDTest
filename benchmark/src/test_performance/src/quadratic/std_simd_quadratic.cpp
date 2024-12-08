#include "../../../../include/core/std_simd_core.h"
#include <nanobench.h>
#include <cmath>
#include <limits>
#include <random>

using ElemType = float;

const size_t kN = (1024 * 1024);
const std::size_t ITERATION = 100;

template <typename Vec, typename Mask, typename Tp>
struct QUADRATIC_SIMD {
void QuadSolveSIMD(const Vec &a, const Vec &b, const Vec &c, Vec &x1, Vec &x2, Vec &roots) {
    Vec vec_zero = details::BroadCast<Vec, Tp>(Tp(0.0));
    Vec vec_one = details::BroadCast<Vec, Tp>(Tp(1.0));
    Vec vec_neg_one = details::BroadCast<Vec, Tp>(Tp(-1.0));
    Vec vec_four = details::BroadCast<Vec, Tp>(Tp(4.0));
    Vec vec_neg_half = details::BroadCast<Vec, Tp>(Tp(-0.5));
    Vec vec_epsilon = details::BroadCast<Vec, Tp>(std::numeric_limits<Tp>::epsilon());

    Vec delta = b * b - vec_four * a * c;

    Vec sqrt_delta = details::Sqrt<Tp>(delta);

    Mask no_real_roots = (delta < vec_zero);
    Mask single_root = (delta < vec_epsilon) & !no_real_roots;
    Mask two_roots = !no_real_roots & !single_root;

    Vec inv_a = vec_one / a;
    Vec root1 = vec_neg_half * (b + sqrt_delta) * inv_a;
    Vec root2 = c / root1;

    details::Select<Mask, Vec, Tp>(two_roots, root1, x1);
    details::Select<Mask, Vec, Tp>(two_roots, root2, x2);
    details::Select<Mask, Vec, Tp>(two_roots, details::BroadCast<Vec, Tp>(Tp(2)), roots);

    Vec single = vec_neg_half * b * inv_a;
    details::Select<Mask, Vec, Tp>(single_root, single, x1);
    details::Select<Mask, Vec, Tp>(single_root, single, x2);
    details::Select<Mask, Vec, Tp>(single_root, vec_one, roots);

    details::Select<Mask, Vec, Tp>(no_real_roots, vec_zero, x1);
    details::Select<Mask, Vec, Tp>(no_real_roots, vec_zero, x2);
    details::Select<Mask, Vec, Tp>(no_real_roots, vec_zero, roots);
  }

void TestQuadSolve(const Tp *a, const Tp *b, const Tp *c, Tp *x1, Tp *x2, Tp *roots) {
    size_t simd_width = details::Len<Vec, Tp>();
    for (size_t i = 0; i < kN; i += simd_width) {
        Vec a_v, b_v, c_v, x1_v, x2_v, roots_v;

        details::Load_Unaligned<Vec, Tp>(a_v, &a[i]);
        details::Load_Unaligned<Vec, Tp>(b_v, &b[i]);
        details::Load_Unaligned<Vec, Tp>(c_v, &c[i]);
        details::Load_Unaligned<Vec, Tp>(x1_v, &x1[i]);
        details::Load_Unaligned<Vec, Tp>(x2_v, &x2[i]);
        details::Load_Unaligned<Vec, Tp>(roots_v, &roots[i]);

        QuadSolveSIMD(a_v, b_v, c_v, x1_v, x2_v, roots_v);

        details::Store_Unaligned<Vec, Tp>(roots_v, &roots[i]);
    }
}
    void operator()(const Tp *a, const Tp *b, const Tp *c, Tp *x1, Tp *x2, Tp *roots) {
        TestQuadSolve(a, b, c, x1, x2, roots);
    }
};

void test_std_simd(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
{
#if defined(USE_PLCT_SIMD)
  QUADRATIC_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("plct_simd", [&]() {
    func(a, b, c, x1, x2, roots);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#else
  QUADRATIC_SIMD<std_simd_t_v_native<ElemType>, std_simd_t_m_native<ElemType>, ElemType> func;
  bench.minEpochIterations(ITERATION).run("std_simd", [&]() {
    func(a, b, c, x1, x2, roots);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#endif
}