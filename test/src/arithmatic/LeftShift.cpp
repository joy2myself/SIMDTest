#include "assert.h"
#include "../../include/test.h"
#include <math.h>


template <typename Tp>
struct test_simd_LeftShift
{
    template <typename Simd>
    void operator()(){
        Tp arr1[details::Len<Simd, Tp>()]{};
        Tp arr2[details::Len<Simd, Tp>()]{};
        Simd tmp1;

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr1[i] = i;
        }

        details::Load_Unaligned(tmp1, arr1);

        for(std::size_t i = 1; i <= details::Len<Simd, Tp>(); ++i){
            arr2[i - 1] = details::Get<Simd, Tp>(details::LeftShift<Simd, Tp>(tmp1 , i), i-1);
        }

        for(std::size_t i = 1; i <= details::Len<Simd, Tp>(); ++i){
            assert(arr2[i - 1] == std::pow(2, i) * arr1[i - 1]);
        }

    }
};

int main(){
    test_all_simd<test_simd_LeftShift<int>, int>();

    // test_all_simd<test_simd_LeftShift<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_LeftShift<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_LeftShift<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_LeftShift<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}