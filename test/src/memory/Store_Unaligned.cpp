#include "assert.h"
#include "../../include/test.h"

template <typename Tp>
struct test_simd_Store_Unaligned
{
    template <typename Simd>
    void operator()(){
        Simd tmp;
        Tp arr1[details::Len<Simd, Tp>()]{};
        Tp arr2[details::Len<Simd, Tp>()]{};
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr1[i] = i;
        }
        details::Load_Unaligned(tmp, arr1);

        details::Store_Unaligned(tmp, arr2);

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            assert((arr1[i] == arr2[i]));
        }
    }
};

int main(){
    test_all_simd<test_simd_Store_Unaligned<float>, float>();
    test_all_simd<test_simd_Store_Unaligned<double>, double>();
    test_all_simd<test_simd_Store_Unaligned<int>, int>();

    // test_all_simd<test_simd_Store_Unaligned<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_Store_Unaligned<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_Store_Unaligned<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_Store_Unaligned<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}