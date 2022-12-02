#include <cmath>
#include <limits>
#include <random>

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

int main()
{

  Initial();
  QUADRATIC_SCALAR{}(a, b, c, x1, x2, roots);
  delete[] a;
  delete[] b;
  delete[] c;
  delete[] x1;
  delete[] x2;
  delete[] roots;
  return 0;
}
