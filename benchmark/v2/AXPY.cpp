/***************************************************************************
 * Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
 * Martin Renou                                                             *
 * Copyright (c) QuantStack                                                 *
 * Copyright (c) Serge Guelton                                              *
 *                                                                          *
 * Distributed under the terms of the BSD 3-Clause License.                 *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

// This file is derived from tsimd (MIT License)
// https://github.com/ospray/tsimd/blob/master/benchmarks/mandelbrot.cpp
// Author Jefferson Amstutz / intel

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <experimental/simd>
#include "../../simd_libraries/version2/vectorclass.h"
#include "../../simd_libraries/xsimd/include/xsimd/xsimd.hpp"
#include "pico_bench.hpp"

namespace ex = std::experimental::parallelism_v2;


// scalar version /////////////////////////////////////////////////////////////

namespace scalar
{

    template <int num>
    void axpy_scalar(float a, float b[num], float c[num],float res[num]){

        for(int i = 0; i < num; ++i){
            res[i] = a * b[i] + c[i];
        }
    }

} // namespace scalar


// mysimd version /////////////////////////////////////////////////////////////
namespace mysimd
{

template <int N,int num>
    void axpy_mysimd(float a, float x[num], float y[num], float res[num]){
        
        std::size_t vec_size = num - num % N;
         ex::fixed_size_simd<float,N> x_simd, y_simd, res_simd;
        for(int i = 0; i < vec_size; i+=N){
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
} // namespace mysimd


// run simd version of AXPY benchmark for a specific arch
template <int N, int num,class bencher_t>
void run_arch(
    bencher_t& bencher,
    float a, 
    float x[num], 
    float y[num], 
    float res[num])
{
    auto stats = bencher([&]()
                         { mysimd::axpy_mysimd<N,num>(a,x,y,res); });

    const float scalar_min = stats.min().count();

    std::cout << '\n'
              << "mysimd N = " << N << " " << stats << '\n';

}


// run simd version of AXPY benchmark for a list
// of archs
template <int num,int... N>
struct run_archlist
{
    template <class bencher_t>
    static void run(
        bencher_t& bencher,
        float a, 
        float x[num], 
        float y[num], 
        float res[num])
    {
        using expand_type = int[];
        expand_type { (run_arch<N,num>(bencher, a,x,y,res), 0)... };

    }
};

namespace VCL{
    template <typename _Tp, int N = 16>
    inline void axpy_VCL(float a, float x[N], float y[N], float res[N]){
        
        std::size_t vec_size = N - N % _Tp().size();

        for(int i = 0; i < vec_size; i+=_Tp().size()){
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
}

namespace _xsimd_{
template <typename arch,int num = 16>
    void axpy(float a, float x[num], float y[num], float res[num]){
        
        std::size_t vec_size = num - num % xsimd::batch<float, arch>::size;
        xsimd::batch<float, arch> res_simd,x_simd,y_simd;
        for(std::size_t i = 0; i < vec_size; i += xsimd::batch<float, arch>::size){
            

            res_simd = a * x_simd.load_aligned(&x[i])+  y_simd.load_aligned(&y[i]);

            res_simd.store_aligned(&res[i]);
        }

        for(std::size_t i = vec_size; i < num; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }
}

int main()
{
    using namespace std::chrono;

    float a = 2;
    float x[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    float y[16] = {17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    float res[16] = {0};

    auto bencher = pico_bench::Benchmarker<nanoseconds> { 64, seconds { 10 } };

    std::cout << "starting benchmarks (results in 'ns')... " << '\n';

    // scalar run ///////////////////////////////////////////////////////////////

    auto stats_scalar = bencher([&]()
                                { scalar::axpy_scalar<16>(a,x,y,res); });

    const float scalar_min = stats_scalar.min().count();

    std::cout << '\n'
              << "scalar " << stats_scalar << '\n';

    // mysimd run ////////////////////////////////////////////////////////////////
    
    run_archlist<16,4>::run(bencher, a,x,y,res);

    // vcl run ////////////////////////////////////////////////////////////////
    // auto stats_VCL_4 = bencher([&]()
    //                             { VCL::axpy_VCL<Vec4i>(a,x,y,res); });

    // const float vcl_4_min = stats_VCL_4.min().count();

    // std::cout << '\n'
    //           << "VCL N=4 " << stats_VCL_4 << '\n';

    // auto stats_VCL_8 = bencher([&]()
    //                             { VCL::axpy_VCL<Vec8i>(a,x,y,res); });

    // const float vcl_8_min = stats_VCL_8.min().count();

    // std::cout << '\n'
    //           << "VCL N=8 " << stats_VCL_8 << '\n';

    //xsimd run ///////////////////////////////////////////////////////////
    // auto stats_xsimd_sse = bencher([&]()
    //                             { _xsimd_::axpy<xsimd::avx2>(a,x,y,res); });

    // const float xsimd_sse_min = stats_xsimd_sse.min().count();

    // std::cout << '\n'
    //           << "xsimd sse " << stats_xsimd_sse << '\n';


    return 0;
}
