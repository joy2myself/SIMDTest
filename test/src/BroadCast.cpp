#include "assert.h"
#include "../include/test.h"


template <typename Tp>
struct test_simd_BroadCast
{
    template <typename Simd>
    void operator()(){
        Simd tmp = details::BroadCast<Simd, Tp>(0.123);
        Tp arr[details::Len<Simd, Tp>()]{};
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr[i] = 0.123;
        }

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            assert((details::Get<Simd, Tp>(tmp, i) == arr[i]));
        }
    }
};

int main(){
    test_all_simd<test_simd_BroadCast<float>, float>();
    test_all_simd<test_simd_BroadCast<double>, double>();
    test_all_simd<test_simd_BroadCast<int>, int>();

    // test_all_simd<test_simd_BroadCast<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_BroadCast<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_BroadCast<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_BroadCast<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}