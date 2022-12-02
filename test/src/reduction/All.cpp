#include "assert.h"
#include "../../include/test.h"


template <typename Tp>
struct test_simd_All
{
    template <typename Simd>
    void operator()(){
        Tp arr1[details::Len<Simd, Tp>()]{};
        Tp arr2[details::Len<Simd, Tp>()]{};
        Simd tmp1;
        Simd tmp2;

        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr1[i] = i;
        }
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            arr2[i] = i * i;
        }

        details::Load_Unaligned(tmp1, arr1);
        details::Load_Unaligned(tmp2, arr2);

        assert(details::All<Tp>(tmp1 < tmp2) == false); 
        assert(details::All<Tp>(tmp1 < details::BroadCast<Simd, Tp>(-1)) == false); 
        assert(details::All<Tp>(tmp1 < details::BroadCast<Simd, Tp>(100)) == true);

    }
};

int main(){
    test_all_simd<test_simd_All<float>, float>();
    test_all_simd<test_simd_All<double>, double>();
    test_all_simd<test_simd_All<int>, int>();

    // test_all_simd<test_simd_All<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_All<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_All<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_All<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}