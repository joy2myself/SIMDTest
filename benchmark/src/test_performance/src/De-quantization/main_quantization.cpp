#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>

using ElemType = int32_t;

///////////////////////parameters initialization////////////////////////

#if defined(__SSE2__)
const std::size_t ARRLENGTH = 64;
#else
const std::size_t ARRLENGTH = 128;
#endif
const std::size_t ITERATION = 5000;

ElemType dct[ARRLENGTH]{ 0 };
ElemType dequant_mf[6][ARRLENGTH]{ 0 };
ElemType res[ARRLENGTH]{ 0 };
ElemType i_qp = rand() % 73;

void Initial()
{
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < ARRLENGTH; ++j)
      dequant_mf[i][j] = rand();
  }

  for (int i = 0; i < ARRLENGTH; ++i)
  {
    dct[i] = rand();
  }
}

void test_scalar(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
void test_vc(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
#endif
void test_highway(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
void test_eve(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType dct[ARRLENGTH], ElemType dequant_mf[6][ARRLENGTH], ElemType i_qp, ElemType res[ARRLENGTH]);

int main()
{
    Initial();
    ankerl::nanobench::Bench b_native;
    b_native.title("De_quantization_TEST_NATIVE").unit("De_quantization_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    test_scalar(b_native, dct, dequant_mf, i_qp, res);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, dct, dequant_mf, i_qp, res);
    // #endif
    
    test_std_simd(b_native, dct, dequant_mf, i_qp, res);
    test_vc(b_native, dct, dequant_mf, i_qp, res);
    test_tsimd(b_native, dct, dequant_mf, i_qp, res);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, dct, dequant_mf, i_qp, res);
    #endif

    if (PLACE_HOLDER){
      test_highway(b_native, dct, dequant_mf, i_qp, res);
      test_xsimd(b_native, dct, dequant_mf, i_qp, res);
      // test_eve(b_native, dct, dequant_mf, i_qp, res);
    }
}