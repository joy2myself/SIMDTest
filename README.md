# simd libraries benchmark

目前只测试了GCC

benchmark/v1：普通版本的测试（无框架）
benchmark/v2：利用了tsimd的测试框架，测试命令为g++ -O2 -march=core-avx2 -std=c++17 <filename>
benchmark/v3：利用了Google benchmark测试框架 g++ -O2 -march=core-avx2 -std=c++17 -pthread -lbenchmark <filename> 

目前已实现的benchmark有两种：AXPY和Mandelbrot；
目前已实现的benchmark的simd版本有：std，xsimd，VCL；





