#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>

using ElemType = float;

const size_t kN = (1024 * 1024);
const std::size_t ITERATION = 100;

ElemType *a = new ElemType[kN];
ElemType *b = new ElemType[kN];
ElemType *c = new ElemType[kN];
ElemType *x1 = new ElemType[kN];
ElemType *x2 = new ElemType[kN];
ElemType *roots = new ElemType[kN];

void test_scalar(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
void test_vc(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
void test_tsimd(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
#endif
void test_highway(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
void test_mipp(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
void test_eve(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);
void test_xsimd(ankerl::nanobench::Bench &bench, const ElemType *a, const ElemType *b, const ElemType *c, ElemType *x1, ElemType *x2, ElemType *roots);

void Initial()
{
  for (size_t i = 0; i < kN; i++)
  {
    a[i] = ElemType(i);
    b[i] = ElemType(i);
    c[i] = 5.0f * ElemType(i);
    x1[i] = 0.0f;
    x2[i] = 0.0f;
    roots[i] = 0;
  }
}


int main()
{
    Initial();

    // 创建 nanobench 测试对象并配置基本参数，设置测试表头标题，启用性能计数器信息
    ankerl::nanobench::Bench b_native;
    b_native.title("quadratic_TEST_NATIVE").unit("quadratic_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    // 使用 nanobench 测试 标量程序 的性能
    test_scalar(b_native, a, b, c, x1, x2, roots);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, a, b, c, x1, x2, roots);
    // #endif
    
    // 使用 nanobench 测试 simd程序 的性能
    test_std_simd(b_native, a, b, c, x1, x2, roots);
    test_vc(b_native, a, b, c, x1, x2, roots);
    test_tsimd(b_native, a, b, c, x1, x2, roots);
    test_mipp(b_native, a, b, c, x1, x2, roots);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, a, b, c, x1, x2, roots);
    #endif

    if (PLACE_HOLDER){
      test_highway(b_native, a, b, c, x1, x2, roots);
      test_xsimd(b_native, a, b, c, x1, x2, roots);
      test_eve(b_native, a, b, c, x1, x2, roots);
    }

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] x1;
    delete[] x2;
    delete[] roots;
    return 0;

}