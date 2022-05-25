#include <experimental/simd>
#include <iostream>
#include <numeric>
#include <chrono>   
#include <cassert>

namespace ex = std::experimental::parallelism_v2;

double average(double *x, int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++) 
        sum += x[i];
    return sum / len; 
}


template <typename _Tp, int N>
inline void axpy_mysimd(int32_t a, int32_t x[N], int32_t y[N], int32_t res[N]){
    
    std::size_t vec_size = N - N % _Tp().size();
    for(int i = 0; i < vec_size; i+=_Tp().size()){
        _Tp x_simd, y_simd, res_simd;
        x_simd.copy_from(&x[i],ex::element_aligned_tag());
        y_simd.copy_from(&y[i],ex::element_aligned_tag());
        res_simd = a * x_simd + y_simd;
        res_simd.copy_to(&res[i],ex::element_aligned_tag());
    }

    for(std::size_t i = vec_size; i < N; ++i)
    {
        res[i] = a * x[i] + y[i];
    }
}

template<typename _Tp, int Times ,int N>
inline void printTimeAndSpeedup(int32_t a, int32_t x[N], int32_t y[N], int32_t res[N]){

    double mysimd_time[Times] = {0}, scalar_time[Times] = {0}, speedup[Times] = {0};
    for(int i = 0; i < Times; ++i){
        auto start_simd = std::chrono::system_clock::now();
        axpy_mysimd<_Tp,N>(a,x,y,res);
        auto end_simd   = std::chrono::system_clock::now();
        auto duration_simd = std::chrono::duration_cast<std::chrono::nanoseconds>(end_simd - start_simd);
        mysimd_time[i] = double(duration_simd.count()) / _Tp().size();
    }

    printf("mysimd average time = %lf ns\n", average(mysimd_time, Times));

}

int main(void)
{
    int a = 2;
    int32_t x[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    int32_t y[16] = {17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    int32_t res[16] = {0};
    printTimeAndSpeedup<ex::fixed_size_simd<int32_t,8>,10000,16>(a,x,y,res);// the first parameter is vector type, the second is times of loop
    return 0;
}