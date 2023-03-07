#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"

#include <climits>
#include <cmath>
#include <experimental/simd>
#include <iostream>
#include <vector>

using ElemType = float;

using namespace std;
// kernal = [3, 3], stride = [1, 1], matrix_size = [m, m]
// Maxpool2D, Averagepool2D, Conv2D, mode = "SAME" / "VALID"
int kernel_size = 4;
int stride = 1;
vector<vector<int>> X = { { 1, 2, 3, 4, 5, 6 }, { 2, 3, 4, 5, 6, 7 },  { 3, 4, 5, 6, 7, 8 },
                          { 4, 5, 6, 7, 8, 9 }, { 5, 6, 7, 8, 9, 10 }, { 6, 7, 8, 9, 10, 11 } };
vector<vector<int>> w(4, vector<int>(4, 1));

template<typename elemType> void showMatrix(vector<vector<elemType>> &matrix)
{
  for (int i = 0; i < matrix.size(); i++)
  {
    for (int j = 0; j < matrix[i].size(); j++)
    {
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

vector<vector<int>> padSame(vector<vector<int>> &X, int kernel_size, int strides)
{
  int out = ceil(float(X.size()) / strides);
  int pad = (out - 1) * strides + kernel_size - X.size();
  int pad_l = floor(pad / 2);
  vector<vector<int>> res(X.size() + pad, vector<int>(X[0].size() + pad, 0));
  for (int i = pad_l; i < X.size() + pad_l; i++)
  {
    for (int j = pad_l; j < X[0].size() + pad_l; j++)
    {
      res[i][j] = X[i - pad_l][j - pad_l];
    }
  }
  return res;
}

vector<vector<int>> padValid(vector<vector<int>> &X, int kernel_size, int strides)
{
  int out = ceil((X.size() - kernel_size + 1) / strides);
  int len = (out - 1) * strides + kernel_size;
  vector<vector<int>> res(len, vector<int>(len, 0));
  for (int i = 0; i < len; i++)
  {
    for (int j = 0; j < len; j++)
    {
      res[i][j] = X[i][j];
    }
  }
  return res;
}

struct CONV_SCALAR
{
  int getMax(vector<vector<int>> &X, int m, int n, int kernel_size, int strides)
  {
    int maxVal = INT_MIN;
    for (int i = m * strides; i < m * strides + kernel_size; i++)
    {
      for (int j = n * strides; j < n * strides + kernel_size; j++)
      {
        maxVal = max(maxVal, X[i][j]);
      }
    }
    return maxVal;
  }

  float getConv(vector<vector<int>> &X, vector<vector<int>> &w, int m, int n, int kernel_size, int strides)
  {
    int sum = 0;
    for (int i = m * strides; i < m * strides + kernel_size; i++)
    {
      for (int j = n * strides; j < n * strides + kernel_size; j++)
      {
        sum += X[i][j] * w[i - m * strides][j - n * strides];
      }
    }
    return sum;
  }

  vector<vector<float>> maxPooling2D(vector<vector<int>> &X, int kernel_size, int strides, string mode)
  {
    vector<vector<int>> padX;
    if (mode == "SAME")
      padX = padSame(X, kernel_size, strides);
    else if (mode == "VALID")
      padX = padValid(X, kernel_size, strides);
    int out = (padX.size() - kernel_size) / strides + 1;
    vector<vector<float>> res(out, vector<float>(out, 0));
    for (int i = 0; i < out; i++)
    {
      for (int j = 0; j < out; j++)
      {
        res[i][j] = getMax(padX, i, j, kernel_size, strides);
      }
    }
    return res;
  }

  vector<vector<float>> conv2D(vector<vector<int>> &X, vector<vector<int>> &w, int kernel_size, int strides, string mode)
  {
    vector<vector<int>> padX;
    if (mode == "SAME")
      padX = padSame(X, kernel_size, strides);
    else if (mode == "VALID")
      padX = padValid(X, kernel_size, strides);
    int out = (padX.size() - kernel_size) / strides + 1;
    vector<vector<float>> res(out, vector<float>(out, 0));

    for (int i = 0; i < out; i++)
    {
      for (int j = 0; j < out; j++)
      {
        res[i][j] = getConv(padX, w, i, j, kernel_size, strides);
      }
    }
    return res;
  }
};

template<typename Vec, typename Tp> struct CONV_SIMD
{
  int getMax(vector<vector<int>> &X, int m, int n, int kernel_size, int strides)
  {
    Vec VmaxVal = details::BroadCast<Vec, Tp>(INT_MIN);
    int maxVal = INT_MIN;
    for (int i = m * strides; i < m * strides + kernel_size; i++)
    {
      Vec a;
      for (int j = n * strides; j < n * strides + kernel_size; j += details::Len<Vec, Tp>())
      {
        details::Load_Unaligned<Vec, Tp>(a, &X[i][j]);
        VmaxVal = details::Max<Tp>(VmaxVal, a);
      }
    }
    for (size_t k = 0; k < details::Len<Vec, Tp>(); k++)
    {
      if (details::Get<Vec, Tp>(VmaxVal, k) > maxVal)
        maxVal = details::Get<Vec, Tp>(VmaxVal, k) ;
    }
    return maxVal;
  }

  float getConv(vector<vector<int>> &X, vector<vector<int>> &w, int m, int n, int kernel_size, int strides)
  {
    Vec Vsum = details::BroadCast<Vec, Tp>(0);
    int sum = 0;
    for (int i = m * strides; i < m * strides + kernel_size; i++)
    {
      Vec Va, Vb;
      for (int j = n * strides; j < n * strides + kernel_size; j += details::Len<Vec, Tp>())
      {
        details::Load_Unaligned(Va, &X[i][j]);
        details::Load_Unaligned(Vb, &w[i - m * strides][j - n * strides]);
        Vsum += Va * Vb;
      }
    }
    // sum = detaisl::reduce(Vsum);
    for (size_t k = 0; k < details::Len<Vec, Tp>(); k++)
    {
      sum += details::Get<Vec, Tp>(Vsum, k);
    }
    return sum;
  }

  vector<vector<float>> maxPooling2D(vector<vector<int>> &X, int kernel_size, int strides, string mode)
  {
    vector<vector<int>> padX;
    if (mode == "SAME")
      padX = padSame(X, kernel_size, strides);
    else if (mode == "VALID")
      padX = padValid(X, kernel_size, strides);
    int out = (padX.size() - kernel_size) / strides + 1;
    vector<vector<float>> res(out, vector<float>(out, 0));
    for (int i = 0; i < out; i++)
    {
      for (int j = 0; j < out; j++)
      {
        res[i][j] = getMax(padX, i, j, kernel_size, strides);
      }
    }
    return res;
  }

  vector<vector<float>> conv2D(vector<vector<int>> &X, vector<vector<int>> &w, int kernel_size, int strides, string mode)
  {
    vector<vector<int>> padX;
    if (mode == "SAME")
      padX = padSame(X, kernel_size, strides);
    else if (mode == "VALID")
      padX = padValid(X, kernel_size, strides);
    int out = (padX.size() - kernel_size) / strides + 1;
    vector<vector<float>> res(out, vector<float>(out, 0));

    for (int i = 0; i < out; i++)
    {
      for (int j = 0; j < out; j++)
      {
        res[i][j] = getConv(padX, w, i, j, kernel_size, strides);
      }
    }
    return res;
  }
};

template<typename F> void bench1(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.title("POOLING_TEST_NATIVE").unit("POOLING_NATIVE").relative(true).performanceCounters(true);
  bench.minEpochIterations(10000).run(name, [&]() { func.maxPooling2D(X, kernel_size, stride, "VALID"); });
}

template<typename F> void bench2(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.title("CONV_TEST_NATIVE").unit("CONV_NATIVE").relative(true).performanceCounters(true);
  bench.minEpochIterations(10000).run(name, [&]() { func.conv2D(X, w, kernel_size, stride, "VALID"); });
}
int main()
{
  cout << "========INPUT========" << endl;

  cout << "kernel_size = " << kernel_size << ", stride = " << stride << endl;
  cout << "Matrix X:" << endl;
  // vector<vector<int>> X;
  // for(int i = 0; i < 1000; i++){
  //     for(int j = 0; j < 1000; j++){
  //         int num = rand();
  //         X[i].push_back(num);
  //     }
  // }
  // for(int i = 0; i < 10; i++){
  //     for(int j = 0; j < 10; j++){
  //         cout<<X[i][j]<<endl;
  //     }
  // }
  showMatrix(X);

  // maxPooling2D
  ankerl::nanobench::Bench b_native;
  bench1<CONV_SCALAR>(b_native, "scalar");
  bench1<CONV_SIMD<std_simd_t_v_native<int>, int>>(b_native, "std_simd");
  bench1<CONV_SIMD<tsimd_t_v_native<int>, int>>(b_native, "tsimd");
  //bench1<CONV_SIMD<vcl_t_v_native<int>, int>>(b_native, "vcl");
  bench1<CONV_SIMD<eve_t_v_native<int>, int>>(b_native, "eve");
  bench1<CONV_SIMD<highway_t_v_native<int>, int>>(b_native, "highway");
  bench1<CONV_SIMD<mipp_t_v_native<int>, int>>(b_native, "mipp");
  bench1<CONV_SIMD<xsimd_t_v_native<int>, int>>(b_native, "xsimd");
  //bench1<CONV_SIMD<nsimd_t_v_native<int>, int>>(b_native, "nsimd");
  bench1<CONV_SIMD<vc_t_v_native<int>, int>>(b_native, "vc");


// conv2D
  ankerl::nanobench::Bench b_native_2;
  bench2<CONV_SCALAR>(b_native_2, "scalar");
  bench2<CONV_SIMD<std_simd_t_v_native<int>, int>>(b_native_2, "std_simd");
  bench2<CONV_SIMD<tsimd_t_v_native<int>, int>>(b_native_2, "tsimd");
  //bench2<CONV_SIMD<vcl_t_v_native<int>, int>>(b_native_2, "vcl");
  bench2<CONV_SIMD<eve_t_v_native<int>, int>>(b_native_2, "eve");
  bench2<CONV_SIMD<highway_t_v_native<int>, int>>(b_native_2, "highway");
  bench2<CONV_SIMD<mipp_t_v_native<int>, int>>(b_native_2, "mipp");
  bench2<CONV_SIMD<xsimd_t_v_native<int>, int>>(b_native_2, "xsimd");
  //bench2<CONV_SIMD<nsimd_t_v_native<int>, int>>(b_native_2, "nsimd");
  bench2<CONV_SIMD<vc_t_v_native<int>, int>>(b_native_2, "vc");

  return 0;
}