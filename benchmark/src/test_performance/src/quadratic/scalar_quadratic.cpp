#include <cstdlib>
#include <nanobench.h>
#include <cmath>
#include <limits>
#include <random>

using ElemType = float;

const size_t kN = (1024 * 1024);
const std::size_t ITERATION = 100;


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

// 使用 nanobench 对标量实现进行性能测试
void test_scalar(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots)
{ 
  QUADRATIC_SCALAR f;
  // 配置 nanobench 的性能测试，指定最少迭代次数，执行标量实现并记录性能结果
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    f(a, b, c, x1, x2, roots);
    ankerl::nanobench::doNotOptimizeAway(f);});
}