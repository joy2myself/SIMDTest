#include <benchmark/benchmark.h>
const std::size_t ARRLENGTH = 128;
using ElemType = float;
void Initial(ElemType a,ElemType* x,ElemType* y,ElemType* res)
{
  a = ElemType(rand());
  for (size_t i = 0; i < ARRLENGTH; ++i)
  {
    x[i] = ElemType(rand());
    y[i] = ElemType(rand());
    res[i] = 0.f;
  }
}

int main(int argc, char **argv) {

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  return 0;
}