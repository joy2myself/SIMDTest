#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>

using ElemType = float;

ElemType xmin = -1.6, xmax = 1.6;
ElemType ymin = -1.6, ymax = 1.6;

size_t nx = 1024, ny = 1024, max_iter = 500;
ElemType cr = -0.123, ci = 0.754;

unsigned char *image = new unsigned char[nx * ny];

void test_scalar(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
void test_vc(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
#endif
void test_highway(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
void test_eve(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, unsigned char *image, ElemType real, ElemType im);

int main()
{
    // 创建 nanobench 测试对象并配置基本参数，设置测试表头标题，启用性能计数器信息
    ankerl::nanobench::Bench b_native;
    b_native.title("julia_TEST_NATIVE").unit("julia_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    // 使用 nanobench 测试 标量程序 的性能
    test_scalar(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
    // #endif
    
    // 使用 nanobench 测试 simd程序 的性能
    test_std_simd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
    test_vc(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
    test_tsimd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
    test_mipp(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
    #endif

    if (PLACE_HOLDER){
      test_highway(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
      test_xsimd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
      test_eve(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image, cr, ci);
    }
}