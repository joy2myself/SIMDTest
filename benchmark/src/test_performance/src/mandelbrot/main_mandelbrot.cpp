#define ANKERL_NANOBENCH_IMPLEMENT
#include <cstdlib>
#include <nanobench.h>

using ElemType = float;

const std::size_t LEN = 4;
const std::size_t ITERATION = 5;

const unsigned int _width = 1024;
const unsigned int _height = 768;
const ElemType x_0 = -2;
const ElemType x_1 = 1;
const ElemType y_0 = -1;
const ElemType y_1 = 1;
const int _maxIters = 256;

std::vector<ElemType> _buf(_width *_height);

void test_scalar(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
void test_vc(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
void test_vcl(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
void test_highway(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
void test_eve(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType x0, ElemType y0, ElemType x1, ElemType y1, int width, int height, int maxIters, std::vector<ElemType> _buf);

int main()
{

    ankerl::nanobench::Bench b_native;
    b_native.title("mandelbrot_TEST_NATIVE").unit("mandelbrot_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    test_scalar(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    // #endif
    
    test_std_simd(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    test_vc(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    test_highway(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    test_tsimd(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    test_mipp(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    test_xsimd(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    test_vcl(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
    test_eve(b_native, x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf);
}