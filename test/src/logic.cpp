#include "assert.h"
#include "../include/test.h"

// TODO: add assert 
template <typename Tp>
struct test_simd_logic
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

        auto and_ = details::And<Tp>(m, m0);
        auto or_ = details::Or<Tp>(m, m0);
        auto not_ = details::Not<Tp>(m); 
        
        auto result0 = details::Select<decltype(m), Simd, Tp>(and_, tmp1, tmp2);
        auto result1 = details::Select<decltype(m), Simd, Tp>(or_, tmp1, tmp2);
        auto result2 = details::Select<decltype(m), Simd, Tp>(not_, tmp1, tmp2);

        std::cout <<"result0: " <<std::endl;
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            std::cout << details::Get<Simd, Tp>(result0, i) << ' ';
        }
        std::cout << std::endl;

        std::cout <<"result1: " <<std::endl;
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            std::cout << details::Get<Simd, Tp>(result1, i) << ' ';
        }
        std::cout << std::endl;

        std::cout <<"result2: " <<std::endl;
        for(std::size_t i = 0; i < details::Len<Simd, Tp>(); ++i){
            std::cout << details::Get<Simd, Tp>(result2, i) << ' ';
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
};

int main(){
    test_all_simd<test_simd_logic<float>, float>();
    test_all_simd<test_simd_logic<double>, double>();
    test_all_simd<test_simd_logic<int>, int>();

    // test_all_simd<test_simd_logic<uint8_t>, uint8_t>();    // mipp::load<uint8_t>  is undefined!
    // test_all_simd<test_simd_logic<uint16_t>, uint16_t>();  // mipp::load<uint16_t>  is undefined!
    // test_all_simd<test_simd_logic<uint32_t>, uint32_t>();  // mipp::load<uint32_t>  is undefined!
    // test_all_simd<test_simd_logic<uint64_t>, uint64_t>();  // mipp::load<uint64_t>  is undefined!
}