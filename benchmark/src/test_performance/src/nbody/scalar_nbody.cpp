#include <cstdlib>
#include <nanobench.h>
using ElemType = float;

const std::size_t ARRLENGTH = 256;
const std::size_t LEN = 4;
const std::size_t ITERATION = 500000;

struct AXPY_SCALAR
{
#ifdef OpenAutoOptimize
  __attribute__((optimize("O2", "tree-vectorize"))) void operator()(ElemType a, ElemType *b, ElemType *c, ElemType *res)
  {
    for (int i = 0; i < ARRLENGTH; ++i)
    {
      res[i] = a * b[i] + c[i];
    }
  }
#else
  void operator()(ElemType a, ElemType *b, ElemType *c, ElemType *res)
  {
    for (int i = 0; i < ARRLENGTH; ++i)
    {
      res[i] = a * b[i] + c[i];
    }
  }
#endif
};

void test_scalar(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res)
{ 
  AXPY_SCALAR f;
  bench.minEpochIterations(ITERATION).run("scalar", [&]() {
    f(a, x, y, res);
    ankerl::nanobench::doNotOptimizeAway(f);});
}