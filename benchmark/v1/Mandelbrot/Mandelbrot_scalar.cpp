
#ifndef __Mandelbrot_scalar__
#define __Mandelbrot_scalar__
#include <iostream>
#include <complex>
#include <numeric>
#include <chrono>   
using namespace std;
using namespace chrono;

double average(double *x, int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++) 
        sum += x[i];
    return sum / len; 
}

inline void mandel_scalar(float c_re, float c_im, int count)
    {
        float z_re = c_re, z_im = c_im;
        int i;
        for (i = 0; i < count; ++i)
        {
            if (z_re * z_re + z_im * z_im > 4.f)
            {
                break;
            }

            float new_re = z_re * z_re - z_im * z_im;
            float new_im = 2.f * z_re * z_im;
            z_re = c_re + new_re;
            z_im = c_im + new_im;
        }
    }

inline void mandelbrot_scalar(float x0, float y0, float x1, float y1,
                    int width, int height, int maxIterations)
    {
        float dx = (x1 - x0) / width;
        float dy = (y1 - y0) / height;

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; ++i)
            {
                float x = x0 + i * dx;
                float y = y0 + j * dy;
                mandel_scalar(x, y, maxIterations);
            }
        }
    }


template<int Times>
inline void printTimeAndSpeedup(float x0, float y0, float x1, float y1, int width, int height, int maxIterations){

    double VCL_time[Times] = {0}, scalar_time[Times] = {0}, speedup[Times] = {0};
    for(int i = 0; i < Times; ++i){

        auto start_scalar = std::chrono::system_clock::now();
        mandelbrot_scalar(x0,y0,x1,y1,width,height,maxIterations);
        auto end_scalar   = std::chrono::system_clock::now();
        auto duration_scalar = std::chrono::duration_cast<std::chrono::microseconds>(end_scalar - start_scalar);
        scalar_time[i] = double(duration_scalar.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    }

    printf(" \n scalar average time = %lf s\n ", average(scalar_time, Times));

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
    printTimeAndSpeedup<10>(x0,y0,x1,y1,width,height,maxIters);//
    return 0;
}
#endif
