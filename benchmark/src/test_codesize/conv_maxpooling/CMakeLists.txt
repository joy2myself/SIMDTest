
set(BENCH_NAME conv_maxPooling)
set(INCLUDE_PATH  ../../../../../simd_libraries/)
set(NSIMD_LIBRARY_PATH  ../../../../../simd_libraries/nsimd)
set(EXECUTABLE_OUTPUT_PATH ./bin/)
add_compile_options(-std=c++2a -O2 -I ${INCLUDE_PATH})

#-------------------------------------------------------------------------------
# sse2
#-------------------------------------------------------------------------------

add_executable(${BENCH_NAME}_sse2_eve eve_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_tsimd tsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_highway highway_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_nsimd nsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_xsimd xsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_std-simd std_simd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_mipp mipp_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_vc vc_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse2_vcl vcl_${BENCH_NAME}.cpp)

target_compile_options(${BENCH_NAME}_sse2_eve PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_tsimd PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_highway PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_nsimd PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_xsimd PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_std-simd PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_mipp PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_vc PRIVATE -march=athlon-fx)
target_compile_options(${BENCH_NAME}_sse2_vcl PRIVATE -march=athlon-fx)

target_compile_options(${BENCH_NAME}_sse2_nsimd PRIVATE -DSSE2 -L${NSIMD_LIBRARY_PATH}/build-sse2-gcc/ -lnsimd_sse2)

#-------------------------------------------------------------------------------
# sse42
#-------------------------------------------------------------------------------

add_executable(${BENCH_NAME}_sse42_eve eve_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_tsimd tsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_highway highway_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_nsimd nsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_xsimd xsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_std-simd std_simd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_mipp mipp_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_vc vc_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_sse42_vcl vcl_${BENCH_NAME}.cpp)

target_compile_options(${BENCH_NAME}_sse42_eve PRIVATE -march=nehalem)
target_compile_options(${BENCH_NAME}_sse42_tsimd PRIVATE -march=nehalem)
target_compile_options(${BENCH_NAME}_sse42_highway PRIVATE -march=nehalem)
# target_compile_options(${BENCH_NAME}_sse42_nsimd PRIVATE -march=nehalem)
target_compile_options(${BENCH_NAME}_sse42_xsimd PRIVATE -march=nehalem)
target_compile_options(${BENCH_NAME}_sse42_std-simd PRIVATE -march=nehalem)
target_compile_options(${BENCH_NAME}_sse42_mipp PRIVATE -march=nehalem)
target_compile_options(${BENCH_NAME}_sse42_vc PRIVATE -march=nehalem)
target_compile_options(${BENCH_NAME}_sse42_vcl PRIVATE -march=nehalem)

# target_compile_options(${BENCH_NAME}_sse42_nsimd PRIVATE -DSSE42 -L${NSIMD_LIBRARY_PATH}/build-sse42-gcc/ -lnsimd_sse42)

#-------------------------------------------------------------------------------
# avx
#-------------------------------------------------------------------------------

add_executable(${BENCH_NAME}_avx_eve eve_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_tsimd tsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_highway highway_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_nsimd nsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_xsimd xsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_std-simd std_simd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_mipp mipp_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_vc vc_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx_vcl vcl_${BENCH_NAME}.cpp)

target_compile_options(${BENCH_NAME}_avx_eve PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_tsimd PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_highway PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_nsimd PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_xsimd PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_std-simd PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_mipp PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_vc PRIVATE -march=sandybridge)
target_compile_options(${BENCH_NAME}_avx_vcl PRIVATE -march=sandybridge)

target_compile_options(${BENCH_NAME}_avx_nsimd PRIVATE -DAVX -L${NSIMD_LIBRARY_PATH}/build-avx-gcc/ -lnsimd_avx)

#-------------------------------------------------------------------------------
# avx2
#-------------------------------------------------------------------------------

add_executable(${BENCH_NAME}_avx2_eve eve_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_tsimd tsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_highway highway_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_nsimd nsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_xsimd xsimd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_std-simd std_simd_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_mipp mipp_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_vc vc_${BENCH_NAME}.cpp)
add_executable(${BENCH_NAME}_avx2_vcl vcl_${BENCH_NAME}.cpp)

target_compile_options(${BENCH_NAME}_avx2_eve PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_tsimd PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_highway PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_nsimd PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_xsimd PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_std-simd PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_mipp PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_vc PRIVATE -march=skylake)
target_compile_options(${BENCH_NAME}_avx2_vcl PRIVATE -march=skylake)

target_compile_options(${BENCH_NAME}_avx2_nsimd PRIVATE -DAVX2 -L${NSIMD_LIBRARY_PATH}/build-avx2-gcc/ -lnsimd_avx2)

