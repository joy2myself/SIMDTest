cmake_minimum_required(VERSION 3.22)
set(BENCH_NAME jacobi2d)
project(${BENCH_NAME}_bench)

set(SIMD_LIBRARY_PATH "../../../../../simd_libraries/" 
                      "../../../../../simd_libraries/eve/include/" 
                      "../../../../../simd_libraries/highway/" 
                    #   "../../../../../simd_libraries/nsimd/include/" 
                      "../../../../../simd_libraries/tsimd/" 
                      "../../../../../simd_libraries/Vc/" 
                      "../../../../../simd_libraries/xsimd/include/" 
                      "../../../../../simd_libraries/nanobench/src/include"
                      "../../../../../simd_libraries/simd_for_godbolt/include/")
set(NSIMD_LIBRARY_PATH  ../../../../../simd_libraries/nsimd)
set(EXECUTABLE_OUTPUT_PATH ./bin/)

if(USE_PLCT_SIMD)
  add_definitions(-DUSE_PLCT_SIMD)
endif()

add_library(${BENCH_NAME}_scalar_object OBJECT
             scalar_${BENCH_NAME}.cpp
            )
target_include_directories(${BENCH_NAME}_scalar_object PRIVATE
            ${SIMD_LIBRARY_PATH})
        
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${BENCH_NAME}_scalar_object PRIVATE -O2 -fno-vectorize)
else()
    target_compile_options(${BENCH_NAME}_scalar_object PRIVATE -O2 -fno-tree-vectorize)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")

    message(STATUS "The current architecture is AArch64 (ARM 64-bit)")

    ##################################################################################################################################

    # neon
    # add AXPY_neon
    add_library(${BENCH_NAME}_neon_object OBJECT
                eve_${BENCH_NAME}.cpp
                vc_${BENCH_NAME}.cpp
                std_simd_${BENCH_NAME}.cpp
                highway_${BENCH_NAME}.cpp
                tsimd_${BENCH_NAME}.cpp
                mipp_${BENCH_NAME}.cpp
                xsimd_${BENCH_NAME}.cpp
                )
    target_include_directories(${BENCH_NAME}_neon_object PRIVATE
                ${SIMD_LIBRARY_PATH})

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${BENCH_NAME}_neon_object PRIVATE --target=aarch64-linux-gnu -march=armv8-a+simd -O2 -fno-vectorize -std=c++2a)
    else()
        target_compile_options(${BENCH_NAME}_neon_object PRIVATE --target=aarch64-linux-gnu -march=armv8-a+simd -O2 -fno-tree-vectorize -std=c++2a)
    endif()
    
    #add executable AXPY_neon
    add_executable(
        ${BENCH_NAME}_neon
        main_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_neon PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_link_libraries(${BENCH_NAME}_neon ${BENCH_NAME}_scalar_object ${BENCH_NAME}_neon_object)

    # ##################################################################################################################################

    # # sve
    # # add AXPY_sve
    # add_library(${BENCH_NAME}_sve_object OBJECT
    #             # eve_${BENCH_NAME}.cpp
    #             vc_${BENCH_NAME}.cpp
    #             std_simd_${BENCH_NAME}.cpp
    #             highway_${BENCH_NAME}.cpp
    #             tsimd_${BENCH_NAME}.cpp
    #             mipp_${BENCH_NAME}.cpp
    #             # xsimd_${BENCH_NAME}.cpp
    #             )
    # target_include_directories(${BENCH_NAME}_sve_object PRIVATE
    #             ${SIMD_LIBRARY_PATH})

    # if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    #     target_compile_options(${BENCH_NAME}_sve_object PRIVATE --target=aarch64-linux-gnu -march=armv8-a+sve -O2 -std=c++2a)
    # else()
    #     target_compile_options(${BENCH_NAME}_sve_object PRIVATE --target=aarch64-linux-gnu -march=armv8-a+sve -O2 -fno-tree-vectorize -std=c++2a)
    # endif()
    

    # set(GENERATED_CPP "main_${BENCH_NAME}_modified_sve.cpp")
    # add_custom_command(
    #     OUTPUT ${GENERATED_CPP}
    #     COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/main_${BENCH_NAME}.cpp | sed 's/define PLACE_HOLDER 1/define PLACE_HOLDER 0/' > ${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_CPP}
    # )
    
    # #add executable AXPY_sve
    # add_executable(
    #     ${BENCH_NAME}_sve
    #     ${GENERATED_CPP}
    # )
    # target_include_directories(${BENCH_NAME}_sve PRIVATE
    #             ${SIMD_LIBRARY_PATH})
    # target_link_libraries(${BENCH_NAME}_sve ${BENCH_NAME}_scalar_object ${BENCH_NAME}_sve_object)

    ##################################################################################################################################

elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")

    message(STATUS "The current architecture is x86_64 (AMD64)")

    # sse2
    # add AXPY_sse2
    add_library(${BENCH_NAME}_sse2_object OBJECT
                eve_${BENCH_NAME}.cpp
                vc_${BENCH_NAME}.cpp
                std_simd_${BENCH_NAME}.cpp
                highway_${BENCH_NAME}.cpp
                tsimd_${BENCH_NAME}.cpp
                mipp_${BENCH_NAME}.cpp
                xsimd_${BENCH_NAME}.cpp
                vcl_${BENCH_NAME}.cpp)
    target_include_directories(${BENCH_NAME}_sse2_object PRIVATE
                ${SIMD_LIBRARY_PATH})

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${BENCH_NAME}_sse2_object PRIVATE -msse2 -O2 -fno-vectorize -std=c++2a)
    else()
        target_compile_options(${BENCH_NAME}_sse2_object PRIVATE -msse2 -O2 -fno-tree-vectorize -std=c++2a)
    endif()

    # # add AXPY_nsimd_sse2 object
    # add_library(${BENCH_NAME}_nsimd_sse2_object OBJECT
    #             nsimd_${BENCH_NAME}.cpp)
    # target_include_directories(${BENCH_NAME}_nsimd_sse2_object PRIVATE
    #             ${SIMD_LIBRARY_PATH})
    # target_compile_options(${BENCH_NAME}_nsimd_sse2_object PRIVATE -msse2 -O2 -std=c++2a -DSSE2 -L${NSIMD_LIBRARY_PATH}/build-sse2-gcc/ -lnsimd_sse2)

    #add executable AXPY_sse2
    add_executable(
        ${BENCH_NAME}_sse2
        main_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_sse2 PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_link_libraries(${BENCH_NAME}_sse2 ${BENCH_NAME}_scalar_object ${BENCH_NAME}_sse2_object)

    ##################################################################################################################################

    # sse42
    # add AXPY_sse42
    add_library(${BENCH_NAME}_sse42_object OBJECT
                eve_${BENCH_NAME}.cpp
                vc_${BENCH_NAME}.cpp
                std_simd_${BENCH_NAME}.cpp
                highway_${BENCH_NAME}.cpp
                tsimd_${BENCH_NAME}.cpp
                mipp_${BENCH_NAME}.cpp
                xsimd_${BENCH_NAME}.cpp
                vcl_${BENCH_NAME}.cpp)
    target_include_directories(${BENCH_NAME}_sse42_object PRIVATE
                ${SIMD_LIBRARY_PATH})

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${BENCH_NAME}_sse42_object PRIVATE -msse4.2 -O2 -fno-vectorize -std=c++2a)
    else()
        target_compile_options(${BENCH_NAME}_sse42_object PRIVATE -msse4.2 -O2 -fno-tree-vectorize -std=c++2a)
    endif()

    #add executable AXPY_sse42
    add_executable(
        ${BENCH_NAME}_sse42
        main_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_sse42 PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_link_libraries(${BENCH_NAME}_sse42 ${BENCH_NAME}_scalar_object ${BENCH_NAME}_sse42_object)
    # target_compile_options(${BENCH_NAME}_sse42 PRIVATE -DNSIMD_INEFFECTIVE)

    ##################################################################################################################################

    # avx
    # add AXPY_avx
    add_library(${BENCH_NAME}_avx_object OBJECT
                eve_${BENCH_NAME}.cpp
                vc_${BENCH_NAME}.cpp
                std_simd_${BENCH_NAME}.cpp
                highway_${BENCH_NAME}.cpp
                tsimd_${BENCH_NAME}.cpp
                mipp_${BENCH_NAME}.cpp
                xsimd_${BENCH_NAME}.cpp
                vcl_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx_object PRIVATE
                ${SIMD_LIBRARY_PATH})

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${BENCH_NAME}_avx_object PRIVATE -mavx -O2 -fno-vectorize -std=c++2a)
    else()
        target_compile_options(${BENCH_NAME}_avx_object PRIVATE -mavx -O2 -fno-tree-vectorize -std=c++2a)
    endif()

    # # add AXPY_nsimd_avx object
    # add_library(${BENCH_NAME}_nsimd_avx_object OBJECT
    #             nsimd_${BENCH_NAME}.cpp)
    # target_include_directories(${BENCH_NAME}_nsimd_avx_object PRIVATE
    #             ${SIMD_LIBRARY_PATH})
    # target_compile_options(${BENCH_NAME}_nsimd_avx_object PRIVATE -mavx -O2 -std=c++2a -DAVX -L${NSIMD_LIBRARY_PATH}/build-avx-gcc/ -lnsimd_avx)

    #add executable AXPY_avx
    add_executable(
        ${BENCH_NAME}_avx
        main_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_link_libraries(${BENCH_NAME}_avx ${BENCH_NAME}_scalar_object ${BENCH_NAME}_avx_object)

    ##################################################################################################################################

    # avx2
    # add AXPY_avx2
    add_library(${BENCH_NAME}_avx2_object OBJECT
                eve_${BENCH_NAME}.cpp
                vc_${BENCH_NAME}.cpp
                std_simd_${BENCH_NAME}.cpp
                highway_${BENCH_NAME}.cpp
                tsimd_${BENCH_NAME}.cpp
                mipp_${BENCH_NAME}.cpp
                xsimd_${BENCH_NAME}.cpp
                vcl_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx2_object PRIVATE
                ${SIMD_LIBRARY_PATH})

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${BENCH_NAME}_avx2_object PRIVATE -mavx2 -O2 -fno-vectorize -std=c++2a)
    else()
        target_compile_options(${BENCH_NAME}_avx2_object PRIVATE -mavx2 -O2 -fno-tree-vectorize -std=c++2a)
    endif()

    # add AXPY_nsimd_avx2 object
    # add_library(${BENCH_NAME}_nsimd_avx2_object OBJECT
    #             nsimd_${BENCH_NAME}.cpp)
    # target_include_directories(${BENCH_NAME}_nsimd_avx2_object PRIVATE
    #             ${SIMD_LIBRARY_PATH})
    # target_compile_options(${BENCH_NAME}_nsimd_avx2_object PRIVATE -mavx2 -O2 -std=c++2a -DAVX2 -L${NSIMD_LIBRARY_PATH}/build-avx2-gcc/ -lnsimd_avx2)

    #add executable AXPY_avx2
    add_executable(
        ${BENCH_NAME}_avx2
        main_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx2 PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_link_libraries(${BENCH_NAME}_avx2 ${BENCH_NAME}_scalar_object ${BENCH_NAME}_avx2_object)

    ##################################################################################################################################

    # avx2_fma
    # add AXPY_avx2_fma
    add_library(${BENCH_NAME}_avx2_fma_object OBJECT
                eve_${BENCH_NAME}.cpp
                vc_${BENCH_NAME}.cpp
                std_simd_${BENCH_NAME}.cpp
                highway_${BENCH_NAME}.cpp
                tsimd_${BENCH_NAME}.cpp
                mipp_${BENCH_NAME}.cpp
                xsimd_${BENCH_NAME}.cpp
                vcl_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx2_fma_object PRIVATE
                ${SIMD_LIBRARY_PATH})

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${BENCH_NAME}_avx2_fma_object PRIVATE -mavx2 -mfma -O2 -fno-vectorize -std=c++2a)
    else()
        target_compile_options(${BENCH_NAME}_avx2_fma_object PRIVATE -mavx2 -mfma -O2 -fno-tree-vectorize -std=c++2a)
    endif()

    # add AXPY_nsimd_avx2_fma object
    # add_library(${BENCH_NAME}_nsimd_avx2_fma_object OBJECT
    #             nsimd_${BENCH_NAME}.cpp)
    # target_include_directories(${BENCH_NAME}_nsimd_avx2_fma_object PRIVATE
    #             ${SIMD_LIBRARY_PATH})
    # target_compile_options(${BENCH_NAME}_nsimd_avx2_fma_object PRIVATE -mavx2 -mfma -O2 -std=c++2a -DAVX2 -L${NSIMD_LIBRARY_PATH}/build-avx2-gcc/ -lnsimd_avx2)

    #add executable AXPY_avx2_fma
    add_executable(
        ${BENCH_NAME}_avx2_fma
        main_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx2_fma PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_link_libraries(${BENCH_NAME}_avx2_fma ${BENCH_NAME}_scalar_object ${BENCH_NAME}_avx2_fma_object)

    ##################################################################################################################################
        # avx512
    # add AXPY_avx512
    add_library(${BENCH_NAME}_avx512_object OBJECT
                eve_${BENCH_NAME}.cpp
                vc_${BENCH_NAME}.cpp
                std_simd_${BENCH_NAME}.cpp
                highway_${BENCH_NAME}.cpp
                tsimd_${BENCH_NAME}.cpp
                mipp_${BENCH_NAME}.cpp
                xsimd_${BENCH_NAME}.cpp
                vcl_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx512_object PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_compile_options(${BENCH_NAME}_avx512_object PRIVATE -mavx512bw -mavx512vl -mavx512f -mavx512cd -mavx512dq -O2 -std=c++2a)

    # add AXPY_nsimd_avx512 object
    # add_library(${BENCH_NAME}_nsimd_avx512_object OBJECT
    #             nsimd_${BENCH_NAME}.cpp)
    # target_include_directories(${BENCH_NAME}_nsimd_avx512_object PRIVATE
    #             ${SIMD_LIBRARY_PATH})
    # target_compile_options(${BENCH_NAME}_nsimd_avx512_object PRIVATE -mavx2 -mfma -O2 -std=c++2a -DAVX2 -L${NSIMD_LIBRARY_PATH}/build-avx2-gcc/ -lnsimd_avx2)

    #add executable AXPY_avx512
    add_executable(
        ${BENCH_NAME}_avx512
        main_${BENCH_NAME}.cpp
    )
    target_include_directories(${BENCH_NAME}_avx512 PRIVATE
                ${SIMD_LIBRARY_PATH})
    target_link_libraries(${BENCH_NAME}_avx512 ${BENCH_NAME}_scalar_object ${BENCH_NAME}_avx512_object)
endif()

