#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>

using ElemType = float;

const std::size_t ITERATION = 500;

struct Color
{
  uint8_t alpha;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};
Color COLORS[5];

void Initial()
{
  COLORS[0] = { 0, 0, 0, 0 };
  COLORS[1] = { 0, 255, 255, 0 };
  COLORS[2] = { 0, 255, 0, 0 };
  COLORS[3] = { 0, 0, 255, 0 };
  COLORS[4] = { 0, 0, 0, 255 };
}


const float xmin = -2, xmax = 2;
const float ymin = -2, ymax = 2;
const size_t nx = 8, ny = 8, max_iter = 4;
Color *image = new Color[nx * ny * 100];



void test_scalar(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
void test_vc(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
#endif
void test_highway(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
void test_eve(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType xmin, ElemType xmax, size_t nx, ElemType ymin, ElemType ymax, size_t ny, size_t max_iter, Color *image);


int main()
{
    Initial();

    // 创建 nanobench 测试对象并配置基本参数，设置测试表头标题，启用性能计数器信息
    ankerl::nanobench::Bench b_native;
    b_native.title("newton_TEST_NATIVE").unit("newton_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    // 使用 nanobench 测试 标量程序 的性能
    test_scalar(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    // #endif
    
    // 使用 nanobench 测试 simd程序 的性能
    test_std_simd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    test_vc(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    test_tsimd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    test_mipp(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    #endif

    if (PLACE_HOLDER){
      test_highway(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
      test_xsimd(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
      test_eve(b_native, xmin, xmax, nx, ymin, ymax, ny, max_iter, image);
    }

    delete[] image;
}