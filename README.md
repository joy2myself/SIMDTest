
# 数据并行编程库通用测试框架

 本通用测试框架是其他矢量化库之上的一个简单抽象层。它提供了一个独立于体系结构的 API，用于表达对数据的向量操作。然后可以将使用此 API 编写的代码分派到使用 Vc、std::simd 、highway等库实现的多个后端之一。这允许人们在 Vc 和 std::simd 等支持的平台上获得最佳性能，而不会失去对不受支持的体系结构（例如 PowerPC）的可移植性。可以切换不同后端而不需要更改用户代码。另一个优点是，与编译器intrinsics函数不同，可以为 SSE、AVX2、AVX512 等编译相同的代码，而无需修改。<br>
目前所支持的benchmark有AXPY、blcakscholes、conv、maxpooling、De-quantization、jacobi2d、julia、mandelbrot、nbody、newton_fractal、quadratic和RGV2YUV 12个benchmark。
 

## SIMD库支持
目前本通用测试框架后端所支持的SIMD库如下，其中EVE和nsimd都用了
c++20新特性，所以需要使用GCC版本在11.2.0之上：

- [Vc](https://github.com/VcDevel/Vc)
- [higway](https://github.com/google/highway)
- [xsimd](https://github.com/xtensor-stack/xsimd)
- [nsimd](https://github.com/agenium-scale/nsimd)
- [vcl](https://github.com/vectorclass)
- [EVE](https://github.com/jfalcou/eve)
- [tsimd](https://github.com/jeffamstutz/tsimd)
- [MIPP](https://github.com/aff3ct/MIPP)
- std::simd

## 架构和指令集支持
本测试框架的架构支持和指令集支持均取决于不同的SIMD后端是否支持。
### 架构支持

- X86
- ARM
- PPC
- RISCV

### 指令集支持
- sse
- avx
- avx2
- avx512
- fma
- neno
- sve
- sve2
- wasm-simd
- altivec
- vmx
- vsx
- amx
- rvv
- ...


## 怎么构建？

```
git clone git@github.com:Panhaolin2001/SIMDTest.git
cd SIMDTest
git submodule update --init --recursive
// build nsimd, 具体请参阅nsimd仓库
cd simd_libraries/nsimd/
bash scripts/build.sh for avx2/avx/sse42/sse2 with gcc
cd ../../benchmark/src/test_performance
mkdir build
cd build
cmake .. -G Ninja
ninja
```
构建成功后，可执行文件在
_./benchmark/src/test_performance/build/XXX/bin/_
目录下，其中
_XXX_
表示不同的benchmark目录。

## Example

benchmark/src 包含的用本测试框架所提供的 API 来编写的benchmark，用于比较不同后端在各种情况下的性能。下面我们展示了如何转换标量函数来计算 AXPY 以使用 SIMD 指令：

### 标量版本
```cpp
struct AXPY_SCALAR
{
 void operator()(ElemType a, ElemType *b, ElemType *c, ElemType *res)
  {
    for (int i = 0; i < ARRLENGTH; ++i)
    {
      res[i] = a * b[i] + c[i];
    }
  }
};
```

### 用本测试框架的API编写的SIMD版本
```cpp
template<typename Vec, typename Tp> struct AXPY_SIMD
{
  void operator()(Tp a, Tp *x, Tp *y, Tp *res)
  {
    auto len = details::Len<Vec, Tp>();
    std::size_t vec_size = ARRLENGTH - ARRLENGTH % len;
    Vec x_simd, y_simd, res_simd;
    for (std::size_t i = 0; i < vec_size; i += len)
    {
      details::Load_Aligned(x_simd, &x[i]);
      details::Load_Aligned(y_simd, &y[i]);
      res_simd = details::BroadCast<Vec, Tp>(a) * x_simd + y_simd;
      details::Store_Aligned(res_simd, &res[i]);
    }
    for (std::size_t i = vec_size; i < ARRLENGTH; ++i)
    {
      res[i] = a * x[i] + y[i];
    }
  }
};
```
