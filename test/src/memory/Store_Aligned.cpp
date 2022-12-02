#include "assert.h"
#include "../../include/test.h"

template <typename Tp>
struct test_simd_Store_Aligned
{
    template <typename Simd>
    void operator()(){
        Simd tmp;
        alignas(64) Tp arr1[details::Len<Simd, Tp>()]{};
        alignas(64) Tp arr2[details::Len<Simd, Tp>()]{};
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr1[i] = i;
        }
        details::Load_Aligned(tmp, arr1);

        details::Store_Aligned(tmp, arr2);

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            assert((arr1[i] == arr2[i]));
        }
    }
};

int main(){
    test_all_simd<test_simd_Store_Aligned<float>, float>();
    test_all_simd<test_simd_Store_Aligned<double>, double>();
    test_all_simd<test_simd_Store_Aligned<int>, int>();

    // test_all_simd<test_simd_Store_Aligned<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_Store_Aligned<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_Store_Aligned<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_Store_Aligned<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}