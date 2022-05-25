#include <experimental/simd>
#include <iostream>
#include <numeric>
#include <chrono> 

namespace ex = std::experimental::parallelism_v2;

double average(double *x, int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++) 
        sum += x[i];
    return sum / len; 
}

template<int N>
    inline void mandel_simd(const ex::fixed_size_simd_mask<float,N>& _active,const ex::fixed_size_simd<float,N>& c_re, const ex::fixed_size_simd<float,N>& c_im, int count)
    {
        ex::fixed_size_simd<float,N> z_re = c_re, z_im = c_im;
       
        for (int i = 0; i < count; ++i)
        {
            auto active = _active & ((z_re * z_re + z_im * z_im) <= ex::fixed_size_simd<float,N>(4.f));
            if (ex::any_of(active))
            {
                break;
            }

            ex::fixed_size_simd<float,N> new_re = z_re * z_re - z_im * z_im;
            ex::fixed_size_simd<float,N> new_im = 2.f * z_re * z_im;
            z_re = c_re + new_re;
            z_im = c_im + new_im;
        }
    }

template<int N>
    inline void mandelbrot_simd(float x0, float y0, float x1, float y1, int width, int height, int maxIterations)
    {
        alignas(32) float tempx[N] = {(x1 - x0) / width};
        alignas(32) float tempy[N] = {(y1 - y0) / height};
        ex::fixed_size_simd<float,N> dx;
        ex::fixed_size_simd<float,N> dy;
        dx.copy_from(tempx,ex::element_aligned_tag());
        dy.copy_from(tempy,ex::element_aligned_tag());

        float arange[N];
        std::iota(&arange[0], &arange[N], 0.f);

        ex::fixed_size_simd<float,N> programIndex;
        programIndex.copy_from(arange,ex::element_aligned_tag());

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i+=N)
            {
                ex::fixed_size_simd<float,N> x = x0 + (i + programIndex) * dx;
                ex::fixed_size_simd<float,N> y = y0 + j * dy;

                auto active = x < ex::fixed_size_simd<float,N>(width);
                mandel_simd<N>(active, x, y, maxIterations);
            }
        }
    }

template<int N, int Times>
inline void printTimeAndSpeedup(float x0, float y0, float x1, float y1, int width, int height, int maxIterations){

    double simd_time[Times] = {0}, scalar_time[Times] = {0}, speedup[Times] = {0};
    for(int i = 0; i < Times; ++i){
        auto start_simd = std::chrono::system_clock::now();
        mandelbrot_simd<N>(x0,y0,x1,y1,width,height,maxIterations);
        auto end_simd   = std::chrono::system_clock::now();
        auto duration_simd = std::chrono::duration_cast<std::chrono::nanoseconds>(end_simd - start_simd);
        simd_time[i] = double(duration_simd.count()) * std::chrono::nanoseconds::period::num / std::chrono::nanoseconds::period::den / N;
    }

    printf(" simd average time = %lf s\n ", average(simd_time, Times));

}

int main(){
    const unsigned int width = 1024;
    const unsigned int height = 768;
    const float x0 = -2;
    const float x1 = 1;
    const float y0 = -1;
    const float y1 = 1;
    const int maxIters = 256;
    printTimeAndSpeedup<4,10>(x0,y0,x1,y1,width,height,maxIters);// the first parameter is the length of simd, the second is times of loop
    return 0;
}