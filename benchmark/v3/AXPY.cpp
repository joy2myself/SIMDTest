#include <benchmark/benchmark.h>
#include <iostream>
#include <experimental/simd>
#include "config.h"

#if HasDefineVCL
#include "../../simd_libraries/version2/vectorclass.h"
#endif

#if HasDefineXsimd
#include "../../simd_libraries/xsimd/include/xsimd/xsimd.hpp"
#endif

#if HasDefineMIPP
#include "../../simd_libraries/MIPP/src/mipp.h"
#endif

#if HasDefineNsimd
#include "../../simd_libraries/nsimd/include/nsimd/nsimd-all.hpp"
#endif

#if HasDefineTsimd
#include "../../simd_libraries/tsimd/tsimd/tsimd.h"
#endif

#if HasDefineEVE
#include "../../simd_libraries/eve/include/eve/wide.hpp"
#include <eve/module/core.hpp>
#endif

namespace ex = std::experimental::parallelism_v2;



// AXPY parameters initialization///////////////////////////////////////
float a = 2;
float x[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
float y[16] = {17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
float res[16] = {0};



// AXPY versiones of simd libraries and scalar///////////////////////////

namespace __scalar_{

    void axpy(float a, float b[16], float c[16],float res[16]){

        for(int i = 0; i < 16; ++i){
            res[i] = a * b[i] + c[i];
        }
    }
}//end scalar namespace


namespace __VCL_{
template <typename _Tp, int N = 16>
    inline void axpy(float a, float x[N], float y[N], float res[N]){
        
        std::size_t vec_size = N - N % _Tp().size();
        _Tp x_vcl, y_vcl, res_vcl, constant(a);
        for(std::size_t i = 0; i < vec_size; i+=_Tp().size()){
           
            x_vcl.load(&x[i]);
            y_vcl.load(&y[i]);
            res_vcl = mul_add(constant,x_vcl,y_vcl);
            //res_vcl = a * x_vcl + y_vcl;
            res_vcl.store(&res[i]);
        }

        for(std::size_t i = vec_size; i < N; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }
}// end VCL namespace


namespace __std_simd_{
template <int N,int num = 16>
    void axpy(float a, float x[num], float y[num], float res[num]){
        
        std::size_t vec_size = num - num % N;
        ex::fixed_size_simd<float,N> x_simd, y_simd, res_simd;
        for(std::size_t i = 0; i < vec_size; i+=N){
            
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


namespace __xsimd_{
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


namespace __nsimd_{


typedef nsimd::pack<f32,4> p_t_4;
typedef nsimd::pack<f32,8> p_t_8;
size_t l_4 = nsimd::len<p_t_4>();
size_t l_8 = nsimd::len<p_t_8>();

void axpy_4(float a, float x[16], float y[16], float res[16]){
    
    std::size_t vec_size = 16 - 16 % l_4;
    p_t_4 x_simd, y_simd, res_simd;
    for(std::size_t i = 0; i < vec_size; i += l_4){
        x_simd = nsimd::loada<p_t_4>(&x[i]);
        y_simd = nsimd::loada<p_t_4>(&y[i]);
        res_simd = a * x_simd + y_simd;
    }

    for(std::size_t i = vec_size; i < 16; ++i)
    {
        res[i] = a * x[i] + y[i];
    }
}

void axpy_8(float a, float x[16], float y[16], float res[16]){
    
    std::size_t vec_size = 16 - 16 % l_8;
    p_t_8 x_simd, y_simd, res_simd;
    for(std::size_t i = 0; i < vec_size; i += l_8){
        x_simd = nsimd::loada<p_t_8>(&x[i]);
        y_simd = nsimd::loada<p_t_8>(&y[i]);
        res_simd = a * x_simd + y_simd;
    }

    for(std::size_t i = vec_size; i < 16; ++i)
    {
        res[i] = a * x[i] + y[i];
    }
}

}// namespace nsimd


namespace __MIPP_{

    void axpy(float a, float x[16], float y[16], float res[16]){
        
        std::size_t vec_size = 16 - 16 % mipp::N<float>();
        
        mipp::Reg<float> x_simd,y_simd,res_simd,constant = a;
        for(std::size_t i = 0; i < vec_size; i += mipp::N<float>()){
            
            x_simd.load(&x[i]);
            y_simd.load(&y[i]);
            res_simd = constant * x_simd + y_simd;
            res_simd.store(&res[i]);
        }

        for(std::size_t i = vec_size; i < 16; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }

}// namespace mipp


namespace __tsimd_{

template<typename T>
    void axpy(float a, float x[16], float y[16], float res[16]){
        
        std::size_t vec_size = 16 - 16 % T::static_size;
        
        for(std::size_t i = 0; i < vec_size; i += T::static_size){
            
            const T x_simd = tsimd::load<T>(&x[i]);
            const T y_simd = tsimd::load<T>(&y[i]);
            const T res_simd = a * x_simd + y_simd; 
            tsimd::store(res_simd, &res[i]);
        }

        for(std::size_t i = vec_size; i < 16; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }
}


namespace __EVE_{

template<int N>
void axpy(float a, float x[16], float y[16], float res[16]){
        
        std::size_t vec_size = 16 - 16 % N;
        
        for(long int i = 0; i < vec_size; i += N){
            eve::wide<float,eve::fixed<N>> x_simd(&x[i]),y_simd(&y[i]),res_simd;
            res_simd = eve::fma(a,x_simd,y_simd);
            eve::store(res_simd,&res[i]);
        }

        for(std::size_t i = vec_size; i < 16; ++i)
        {
            res[i] = a * x[i] + y[i];
        }
    }

}// namespace eve



// benchmark function///////////////////////////////////////////////////////
static void BM_AXPY_SCALAR(benchmark::State& state) {
    for (auto _ : state)
        __scalar_::axpy(a,x,y,res);
}

template <int N>
static void BM_AXPY_std_simd(benchmark::State& state) {
    for (auto _ : state)
        __std_simd_::axpy<N>(a,x,y,res);
}

template <typename _Tp>
static void BM_AXPY_VCL(benchmark::State& state) {
    for (auto _ : state)
        __VCL_::axpy<_Tp>(a,x,y,res);
}

template <typename arch>
static void BM_AXPY_xsimd(benchmark::State& state) {
    for (auto _ : state)
        __xsimd_::axpy<arch>(a,x,y,res);
}

static void BM_AXPY_mipp(benchmark::State& state) {
    for (auto _ : state)
        __MIPP_::axpy(a,x,y,res);
}

static void BM_AXPY_nsimd_4(benchmark::State& state) {
    for (auto _ : state)
        __nsimd_::axpy_4(a,x,y,res);
}

static void BM_AXPY_nsimd_8(benchmark::State& state) {
    for (auto _ : state)
        __nsimd_::axpy_8(a,x,y,res);
}

template <typename _Tp>
static void BM_AXPY_tsimd(benchmark::State& state) {
    for (auto _ : state)
        __tsimd_::axpy<_Tp>(a,x,y,res);
}

template <int N>
static void BM_AXPY_eve(benchmark::State& state) {
    for (auto _ : state)
        __EVE_::axpy<N>(a,x,y,res);
}


//benchmarking///////////////////////////////////////////////////////////////

BENCHMARK(BM_AXPY_SCALAR);

BENCHMARK(BM_AXPY_mipp);

// BENCHMARK(BM_AXPY_nsimd_4);
// BENCHMARK(BM_AXPY_nsimd_8);

BENCHMARK_TEMPLATE(BM_AXPY_VCL, Vec4f);
BENCHMARK_TEMPLATE(BM_AXPY_VCL, Vec8f);

BENCHMARK_TEMPLATE(BM_AXPY_eve, 4);
BENCHMARK_TEMPLATE(BM_AXPY_eve, 8);

BENCHMARK_TEMPLATE(BM_AXPY_std_simd, 4);
BENCHMARK_TEMPLATE(BM_AXPY_std_simd, 8);

// BENCHMARK_TEMPLATE(BM_AXPY_xsimd, xsimd::sse4_2);
// BENCHMARK_TEMPLATE(BM_AXPY_xsimd, xsimd::avx2);

BENCHMARK_TEMPLATE(BM_AXPY_tsimd, tsimd::vfloat4);
BENCHMARK_TEMPLATE(BM_AXPY_tsimd, tsimd::vfloat8);

BENCHMARK_MAIN();