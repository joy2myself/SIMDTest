#define ANKERL_NANOBENCH_IMPLEMENT
#include <cstdlib>
#include <nanobench.h>

using ElemType = float;

const std::size_t ARRLENGTH = 256;
const std::size_t LEN = 4;
const std::size_t ITERATION = 500000;

ElemType a;
alignas(32) ElemType x[ARRLENGTH]{ 0 };
alignas(32) ElemType y[ARRLENGTH]{ 0 };
alignas(32) ElemType res[ARRLENGTH]{ 0 };

void test_scalar(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
void test_vc(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
void test_vcl(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
void test_highway(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
void test_eve(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType a, ElemType *x, ElemType *y, ElemType *res);

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

int main()
{
    Initial();

    ankerl::nanobench::Bench b_native;
    b_native.title("AXPY_TEST_NATIVE").unit("AXPY_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    test_scalar(b_native, a, x, y, res);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, a, x, y, res);
    // #endif
    
    test_std_simd(b_native, a, x, y, res);
    test_vc(b_native, a, x, y, res);
    test_highway(b_native, a, x, y, res);
    test_tsimd(b_native, a, x, y, res);
    test_mipp(b_native, a, x, y, res);
    test_xsimd(b_native, a, x, y, res);
    test_vcl(b_native, a, x, y, res);
    test_eve(b_native, a, x, y, res);
}