
#ifndef __AXPY_SCALAR__
#define __AXPY_SCALAR__
#include <cstdint>

template <int N>
void axpy_scalar(int32_t a, int32_t b[N], int32_t c[N],int32_t res[N]){

    for(int i = 0; i < N; ++i){
        res[i] = a * b[i] + c[i];
    }
}
#endif