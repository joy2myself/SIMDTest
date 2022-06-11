
#ifndef __AXPY_SCALAR__
#define __AXPY_SCALAR__
#include <cstdint>
#include <experimental/simd>
#include <iostream>
#include <numeric>
#include <chrono>   
#include <cassert>

double average(double *x, int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++) 
        sum += x[i];
    return sum / len; 
}

template <int N>
void axpy_scalar(int32_t a, int32_t b[N], int32_t c[N],int32_t res[N]){

    for(int i = 0; i < N; ++i){
        res[i] = a * b[i] + c[i];
    }
}

template<int Times ,int N>
inline void printTimeAndSpeedup(int32_t a, int32_t x[N], int32_t y[N], int32_t res[N]){

    double scalar_time[Times] = {0};
    for(int i = 0; i < Times; ++i){
        auto start_scalar = std::chrono::system_clock::now();
        axpy_scalar<N>(a,x,y,res);
        auto end_scalar   = std::chrono::system_clock::now();
        auto duration_scalar = std::chrono::duration_cast<std::chrono::nanoseconds>(end_scalar - start_scalar);
        scalar_time[i] = double(duration_scalar.count()); 
    }

    printf(" \n scalar average time = %lf ns\n ", average(scalar_time, Times));

}

int main(void)
{
    int a = 2;
    int32_t x[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    int32_t y[16] = {17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    int32_t res[16] = {0};
    printTimeAndSpeedup<10000,16>(a,x,y,res);// the first parameter is vector type, the second is times of loop
    return 0;
}
#endif