#include <benchmark/benchmark.h>
#include <iostream>
#include <experimental/simd>
#include "../../simd_libraries/version2 copy/include/vectorclass.h"
#include "../../simd_libraries/xsimd copy/include/xsimd/xsimd.hpp"

namespace ex = std::experimental::parallelism_v2;


int a = 2;
int32_t x[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
int32_t y[16] = {17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
int32_t res[16] = {0};


namespace scalar{

    void axpy(int32_t a, int32_t b[16], int32_t c[16],int32_t res[16]){

        for(int i = 0; i < 16; ++i){
            res[i] = a * b[i] + c[i];
        }
    }
}//end scalar namespace

namespace VCL{
template <typename _Tp, int N = 16>
    inline void axpy(int32_t a, int32_t x[N], int32_t y[N], int32_t res[N]){
        
        std::size_t vec_size = N - N % _Tp().size();

        for(std::size_t i = 0; i < vec_size; i+=_Tp().size()){
            _Tp x_vcl, y_vcl, res_vcl;
            x_vcl.load(&x[i]);
            y_vcl.load(&y[i]);
            res_vcl = a * x_vcl + y_vcl;
            res_vcl.store(&res[i]);
        }

        for(std::size_t i = vec_size; i < N; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }
}// end VCL namespace

namespace mysimd{
template <int N,int num = 16>
    void axpy(int32_t a, int32_t x[num], int32_t y[num], int32_t res[num]){
        
        std::size_t vec_size = num - num % N;
        for(std::size_t i = 0; i < vec_size; i+=N){
            ex::fixed_size_simd<int,N> x_simd, y_simd, res_simd;
            x_simd.copy_from(&x[i],ex::element_aligned_tag());
            y_simd.copy_from(&y[i],ex::element_aligned_tag());
            res_simd = a * x_simd + y_simd;
            res_simd.copy_to(&res[i],ex::element_aligned_tag());
        }

        for(std::size_t i = vec_size; i < num; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }
}//end mysimd namespace

namespace x_simd{
template <typename arch,int num = 16>
    void axpy(int a, int x[num], int y[num], int res[num]){
        
        std::size_t vec_size = num - num % xsimd::batch<int, arch>::size;
        for(std::size_t i = 0; i < vec_size; i += xsimd::batch<int, arch>::size){
            xsimd::batch<int, arch> res_simd,x_simd,y_simd;

            res_simd = a * x_simd.load_aligned(&x[i])+  y_simd.load_aligned(&y[i]);

            res_simd.store_aligned(&res[i]);
        }

        for(std::size_t i = vec_size; i < num; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }
}

static void BM_AXPY_SCALAR(benchmark::State& state) {
    for (auto _ : state)
        scalar::axpy(a,x,y,res);
}

template <int N>
static void BM_AXPY_MYSIMD(benchmark::State& state) {
    for (auto _ : state)
        mysimd::axpy<N>(a,x,y,res);
}

template <typename _Tp>
static void BM_AXPY_VCL(benchmark::State& state) {
    for (auto _ : state)
        VCL::axpy<_Tp>(a,x,y,res);
}

template <typename arch>
static void BM_AXPY_xsimd(benchmark::State& state) {
    for (auto _ : state)
        x_simd::axpy<arch>(a,x,y,res);
}

BENCHMARK(BM_AXPY_SCALAR);

BENCHMARK_TEMPLATE(BM_AXPY_VCL, Vec4i);
BENCHMARK_TEMPLATE(BM_AXPY_VCL, Vec8i);

BENCHMARK_TEMPLATE(BM_AXPY_MYSIMD, 4);
BENCHMARK_TEMPLATE(BM_AXPY_MYSIMD, 8);

BENCHMARK_TEMPLATE(BM_AXPY_xsimd, xsimd::sse4_2);
BENCHMARK_TEMPLATE(BM_AXPY_xsimd, xsimd::avx2);

BENCHMARK_MAIN();