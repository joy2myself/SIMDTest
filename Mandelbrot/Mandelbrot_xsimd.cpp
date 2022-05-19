#include "../simd_library/xsimd/xsimd/include/xsimd/xsimd.hpp"
#include <iostream>
#include <complex>
#include <numeric>
#include <chrono>   
#include "Mandelbrot_scalar.cpp"
using namespace std;
using namespace chrono;
namespace xs = xsimd;

inline double average(double *x, int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++) 
        sum += x[i];
    return sum / len; 
}

 template <class arch>
    inline void mandel_xsimd(const xs::batch_bool<float, arch>& _active,
                                   const xs::batch<float, arch>& c_re,
                                   const xs::batch<float, arch>& c_im,
                                   int maxIters)
    {
        using float_batch_type = xs::batch<float, arch>;
        using int_batch_type = xs::batch<int, arch>;

        constexpr std::size_t N = float_batch_type::size;

        float_batch_type z_re = c_re;
        float_batch_type z_im = c_im;

        for (int i = 0; i < maxIters; ++i)
        {
            auto active = _active & ((z_re * z_re + z_im * z_im) <= float_batch_type(4.f));
            if (!xsimd::any(active))
            {
                break;
            }

            float_batch_type new_re = z_re * z_re - z_im * z_im;
            float_batch_type new_im = 2.f * z_re * z_im;

            z_re = c_re + new_re;
            z_im = c_im + new_im;
        }
    }

    template <class arch>
    inline void mandelbrot_xsimd(float x0,
                    float y0,
                    float x1,
                    float y1,
                    int width,
                    int height,
                    int maxIters)
    {
        using float_batch_type = xs::batch<float, arch>;

        constexpr std::size_t N = float_batch_type::size;
        float dx = (x1 - x0) / width;
        float dy = (y1 - y0) / height;

        float arange[N];
        std::iota(&arange[0], &arange[N], 0.f);
        auto programIndex = float_batch_type::load(&arange[0], xsimd::aligned_mode());

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i += N)
            {
                float_batch_type x(x0 + (i + programIndex) * dx);
                float_batch_type y(y0 + j * dy);

                auto active = x < float_batch_type(width);
                mandel_xsimd<arch>(active, x, y, maxIters);
            }
        }
    }

 
template<typename Arch, int Times>
inline void printTimeAndSpeedup(float x0, float y0, float x1, float y1, int width, int height, int maxIterations){

    double xsimd_time[Times] = {0}, scalar_time[Times] = {0}, speedup[Times] = {0};
    for(int i = 0; i < Times; ++i){
        auto start_simd = std::chrono::system_clock::now();
        mandelbrot_xsimd<Arch>(x0,y0,x1,y1,width,height,maxIterations);
        auto end_simd   = std::chrono::system_clock::now();
        auto duration_simd = std::chrono::duration_cast<std::chrono::microseconds>(end_simd - start_simd);
        xsimd_time[i] = double(duration_simd.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den / xs::batch<float, Arch>::size;

        auto start_scalar = std::chrono::system_clock::now();
        mandelbrot_scalar(x0,y0,x1,y1,width,height,maxIterations);
        auto end_scalar   = std::chrono::system_clock::now();
        auto duration_scalar = std::chrono::duration_cast<std::chrono::microseconds>(end_scalar - start_scalar);
        scalar_time[i] = double(duration_scalar.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;

        speedup[i] = scalar_time[i] / xsimd_time[i];
    }

    printf(" \n scalar average time = %lf\n xsimd average time = %lf\n average speedup = %lf\n", average(scalar_time, Times),average(xsimd_time, Times),average(speedup, Times));

}

int main(void)
{
    const unsigned int width = 1024;
    const unsigned int height = 768;
    const float x0 = -2;
    const float x1 = 1;
    const float y0 = -1;
    const float y1 = 1;
    const int maxIters = 256;
    printTimeAndSpeedup<xs::sse2,10>(x0,y0,x1,y1,width,height,maxIters);// the first parameter is ISA, the second is times of loop
    return 0;
}