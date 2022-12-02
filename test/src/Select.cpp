#include "assert.h"
#include "../include/test.h"

template <typename Tp>
struct test_simd_Select
{
    template <typename Simd>
    void operator()(){
        Tp arr[details::Len<Simd, Tp>()]{};
        Tp arr1[details::Len<Simd, Tp>()]{};
        Tp arr2[details::Len<Simd, Tp>()]{};
        Simd tmp;
        Simd tmp1;
        Simd tmp2;

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr1[i] = i * i;
        }
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr2[i] = 2 * i;
        }

        details::Load_Unaligned(tmp1, arr1);
        details::Load_Unaligned(tmp2, arr2);
        auto m = tmp1 < tmp2;

        auto m0 = details::BroadCast<decltype(m), Tp>(true);
        tmp = details::Select<decltype(m0), Simd, Tp>(m0, tmp1, tmp2);
        
        details::Store_Unaligned(tmp, arr);

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            assert(arr[i] == arr1[i]);
        }
    }
};

int main(){
    test_all_simd<test_simd_Select<float>, float>();
    test_all_simd<test_simd_Select<double>, double>();
    test_all_simd<test_simd_Select<int>, int>();

    // test_all_simd<test_simd_Select<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_Select<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_Select<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_Select<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}