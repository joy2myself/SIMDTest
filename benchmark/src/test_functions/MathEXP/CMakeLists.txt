set(BENCH_NAME MathExp)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ ISO Standard")
set(NSIMD_LIBRARY_PATH  ../../../../../simd_libraries/nsimd)
set(EXECUTABLE_OUTPUT_PATH ../bin/)

option(ISAFLAG "ISA or march flag")


# add_library(${BENCH_NAME}_nsimd_object STATIC
#             nsimd.cpp)
# target_include_directories(${BENCH_NAME}_nsimd_object PRIVATE ${INSTALL_DIR}/include)
# set_target_properties(${BENCH_NAME}_nsimd_object PROPERTIES LINKER_LANGUAGE CXX)

# if(${ISAFLAG} STREQUAL "-msse2")
# message("Now the ISA is sse2")
# target_compile_options(${BENCH_NAME}_nsimd_object PRIVATE 
#                        ${ISAFLAG} -DSSE2 -L${NSIMD_LIBRARY_PATH}/build-sse2-gcc/ 
#                        -lnsimd_sse2)
# endif()

# if(${ISAFLAG} STREQUAL "-msse4.2")
# message("Now the ISA is sse4.2")
# target_compile_options(${BENCH_NAME}_nsimd_object PRIVATE 
#                        ${ISAFLAG} -DSSE42 -L${NSIMD_LIBRARY_PATH}/build-sse42-gcc/ 
#                        -lnsimd_sse42)
# endif()

# if(${ISAFLAG} STREQUAL "-mavx")
# message("Now the ISA is avx")
# target_compile_options(${BENCH_NAME}_nsimd_object PRIVATE 
#                        ${ISAFLAG} -DAVX -L${NSIMD_LIBRARY_PATH}/build-avx-gcc/ 
#                        -lnsimd_avx)
# endif()

# if(${ISAFLAG} STREQUAL "-mavx2")
# message("Now the ISA is avx2")
# target_compile_options(${BENCH_NAME}_nsimd_object PRIVATE 
#                        ${ISAFLAG} -DAVX2 -L${NSIMD_LIBRARY_PATH}/build-avx2-gcc/ 
#                        -lnsimd_avx2)
# endif()

add_executable(${BENCH_NAME}-benchmark
  main.cpp
  eve.cpp
  vc.cpp
  std_simd.cpp
  highway.cpp
  tsimd.cpp
  mipp.cpp
  xsimd.cpp
  vcl.cpp
  # nsimd.cpp
)

target_compile_options(${BENCH_NAME}-benchmark PRIVATE ${ISAFLAG})
target_include_directories(${BENCH_NAME}-benchmark PRIVATE ${INSTALL_DIR}/include)
target_include_directories(${BENCH_NAME}-benchmark PRIVATE /home/panhaolin/phl/simd-libraries-benchmark/simd_libraries/)
set_target_properties(${BENCH_NAME}-benchmark PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${BENCH_NAME}-benchmark
  GoogleBenchmark
  # ${BENCH_NAME}_nsimd_object
  )