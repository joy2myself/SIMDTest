#include <cstdlib>
using ElemType = float;

const std::size_t ARRLENGTH = 256;
const std::size_t LEN = 4;
const std::size_t ITERATION = 500000;

ElemType a;
ElemType x[ARRLENGTH]{ 0 };
ElemType y[ARRLENGTH]{ 0 };
ElemType res[ARRLENGTH]{ 0 };

void Initial()
{
  a = ElemType(rand());
  for (size_t i = 0; i < ARRLENGTH; ++i)
  {
    x[i] = ElemType(rand());
    y[i] = ElemType(rand());
    res[i] = 0.f;
  }
}

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

int main()
{
  Initial();
  AXPY_SCALAR{}(a, x, y, res);
}