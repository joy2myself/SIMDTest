#include "../../benchmark/all.h"

template <typename F>
void test_simd() {}

template <typename F, typename Simd, typename... Simds>
void test_simd() {
  F{}.template operator()<Simd>();
  test_simd<F, Simds...>();
}

template <typename F, typename Tp>
void test_all_simd() {
  test_simd<F,
    // eve_t_v_native<Tp>,
    // highway_t_v_native<Tp>, // TODO: highway need add -march option, only -mxxx does not work.
    // mipp_t_v_native<Tp>,
    // nsimd_t_v_native<Tp>, // TODO: nsimd fallback to scalar.
    // std_simd_t_v_native<Tp>
    tsimd_t_v_native<Tp> // TODO: static assertion failed: pack 'T' type currently must be 'float', 'int', 'double', 'long long', 'bool32_t', or 'bool64_t'!
    // vcl_t_v_native<Tp>,
    // xsimd_t_v_native<Tp>
    // ume_simd_t_v_native<Tp>, // TODO: implementation
    // vc_t_v_native<Tp>
  >();
}

