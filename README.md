
# README

## 说明

注意在dev分支上提交benchmark

v2版本使用的是tsimd测试框架来测试的。

v3版本使用的是google benchmark测试框架来测试的。

v4版本使用的是nanobench测试框架来测试的。

目前最稳定的版本是使用nanobench的v4版本。

## 怎么测试？

克隆之后，把submodule克隆下来

```cpp
$git clone git@yt.droid.ac.cn:panhaolin/simd-libraries-benchmark.git
$cd simd-libraries-benchmark
$git checkout dev
$git submodule update --init --recursive
```

目前，v3版本和v4版本目录下有command.txt文件来描述运行命令。
例如在v4中：

```cpp
//使用自己得库
clang++ -O2 -march=corei7-avx -mavx2 -mfma -I../../nanobench/src/include mandelbrot.cpp                                  \
	-nostdinc++ -nostdlib++ -isystem /*PATH*/llvm-project/build/include/c++/v1 -L  /usr/lib -Wl,-rpath, /usr/lib         \
	-lc++ -std=c++20

g++ -O2 -march=corei7-avx -mavx2 -mfma   -I../../nanobench/src/include mandelbrot.cpp                                    \
	-std=c++20 -nostdinc++ -nodefaultlibs -isystem /*PATH*/llvm-project/build/include/c++/v1 -L /usr/lib                 \
	-Wl,-rpath,/usr/lib -lc++ -lc++abi -lm -lc -lgcc_s -lgcc

// *PATH*为你从llvm-project克隆下来存放的地址

//使用标准库libstdc++也就是 std_simd

g++ -O2 -std=c++20  -march=corei7-avx -mavx2 -mfma -I../../../nanobench-4.3.7/src/include mandelbrot.cpp

clang++ -O2 -std=c++20  -march=corei7-avx -mavx2 -mfma -I../../../nanobench-4.3.7/src/include mandelbrot.cpp

```

同理可以查看v3目录下的command.txt来确定google benchmark的运行命令。

## 性能分析目录

    主要是利用gperftools工具来查看函数调用情况以及调用过程中看哪些调用消耗时间最大，从而找到可以优化的点。