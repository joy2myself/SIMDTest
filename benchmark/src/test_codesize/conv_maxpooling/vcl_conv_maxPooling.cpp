#include "../../../include/core/vcl_core.h"

#include <climits>
#include <cmath>
#include <iostream>
#include <vector>

using ElemType = float;

using namespace std;
int kernel_size = 4;
int stride = 1;
vector<vector<int>> X = { { 1, 2, 3, 4, 5, 6 }, { 2, 3, 4, 5, 6, 7 },  { 3, 4, 5, 6, 7, 8 },
                          { 4, 5, 6, 7, 8, 9 }, { 5, 6, 7, 8, 9, 10 }, { 6, 7, 8, 9, 10, 11 } };
vector<vector<int>> w(4, vector<int>(4, 1));

template<typename ElemType> void showMatrix(vector<vector<ElemType>> &matrix)
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

int main()
{
  cout << "========INPUT========" << endl;

  cout << "kernel_size = " << kernel_size << ", stride = " << stride << endl;
  cout << "Matrix X:" << endl;
  showMatrix(X);

  // maxPooling2D
  CONV_SIMD<vcl_t_v_native<int>, int>{}.maxPooling2D(X, kernel_size, stride, "VALID");

  // conv2D
  CONV_SIMD<vcl_t_v_native<int>, int>{}.conv2D(X, w, kernel_size, stride, "VALID");

  return 0;
}