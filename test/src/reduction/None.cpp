#include "assert.h"
#include "../../include/test.h"


template <typename Tp>
struct test_simd_None
{
    template <typename Simd>
    void operator()(){
        Tp arr1[details::Len<Simd, Tp>()]{};
        Tp arr2[details::Len<Simd, Tp>()]{};
        Simd tmp1;
        Simd tmp2;

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr1[i] = -1;
        }
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr2[i] = i * i;
        }

        details::Load_Unaligned(tmp1, arr1);
        details::Load_Unaligned(tmp2, arr2);

        assert(details::None<Tp>(tmp1 < tmp2) == false); 
        assert(details::None<Tp>(tmp1 < details::BroadCast<Simd, Tp>(-1)) == true); 
        assert(details::None<Tp>(tmp2 < details::BroadCast<Simd, Tp>(2)) == false);
    }
};

int main(){
    test_all_simd<test_simd_None<float>, float>();
    test_all_simd<test_simd_None<double>, double>();
    test_all_simd<test_simd_None<int>, int>();

    // test_all_simd<test_simd_None<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_None<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_None<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_None<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}