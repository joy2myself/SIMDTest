/***************************************************************************
 * Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
 * Martin Renou                                                             *
 * Copyright (c) QuantStack                                                 *
 * Copyright (c) Serge Guelton                                              *
 *                                                                          *
 * Distributed under the terms of the BSD 3-Clause License.                 *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

// This file is derived from tsimd (MIT License)
// https://github.com/ospray/tsimd/blob/master/benchmarks/mandelbrot.cpp
// Author Jefferson Amstutz / intel

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <experimental/simd>
#include "../../simd_libraries/version2/vectorclass.h"
#include "../../simd_libraries/xsimd/include/xsimd/xsimd.hpp"
#include "pico_bench.hpp"

namespace ex = std::experimental::parallelism_v2;

// helper function to write the rendered image as PPM file
inline void writePPM(const std::string& fileName,
                     const int sizeX,
                     const int sizeY,
                     const int* pixel)
{
    FILE* file = fopen(fileName.c_str(), "wb");
    fprintf(file, "P6\n%i %i\n255\n", sizeX, sizeY);
    unsigned char* out = (unsigned char*)alloca(3 * sizeX);
    for (int y = 0; y < sizeY; y++)
    {
        const unsigned char* in = (const unsigned char*)&pixel[(sizeY - 1 - y) * sizeX];

        for (int x = 0; x < sizeX; x++)
        {
            out[3 * x + 0] = in[4 * x + 0];
            out[3 * x + 1] = in[4 * x + 1];
            out[3 * x + 2] = in[4 * x + 2];
        }

        fwrite(out, 3 * sizeX, sizeof(char), file);
    }
    fprintf(file, "\n");
    fclose(file);
}

// scalar version /////////////////////////////////////////////////////////////

namespace scalar
{

    inline int mandel(float c_re, float c_im, int count)
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

        return i;
    }

    void mandelbrot(float x0, float y0, float x1, float y1,
                    int width, int height, int maxIterations, int output[])
    {
        float dx = (x1 - x0) / width;
        float dy = (y1 - y0) / height;

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; ++i)
            {
                float x = x0 + i * dx;
                float y = y0 + j * dy;

                int index = (j * width + i);
                output[index] = mandel(x, y, maxIterations);
            }
        }
    }

} // namespace scalar

namespace mysimd
{

template<int N>
    inline ex::fixed_size_simd<float,N> mandel(const ex::fixed_size_simd_mask<float,N>& _active,
                            const ex::fixed_size_simd<float,N>& c_re, 
                            const ex::fixed_size_simd<float,N>& c_im, 
                            int maxIters)
    {
        ex::fixed_size_simd<float,N> z_re = c_re;
        ex::fixed_size_simd<float,N> z_im = c_im;
        ex::fixed_size_simd<float,N> vi(0);

        for (int i = 0; i < maxIters; ++i)
        {
            ex::fixed_size_simd_mask<float,N> active = _active & ((z_re * z_re + z_im * z_im) <= ex::fixed_size_simd<float,N>(4.f));
            if (!ex::any_of(active))
            {
                break;
            }

            ex::fixed_size_simd<float,N> new_re = z_re * z_re - z_im * z_im;
            ex::fixed_size_simd<float,N> new_im = 2.f * z_re * z_im;
            z_re = c_re + new_re;
            z_im = c_im + new_im;

            ex::where(active, vi)++;
        }
        return vi;
    }

template<int N>
    inline void mandelbrot(float x0, float y0, float x1, float y1, int width, int height, int maxIters, int output[])
    {
        float dx = (x1 - x0) / width;
        float dy = (y1 - y0) / height;
        
        float arange[N];
        std::iota(&arange[0], &arange[N], 0.f);

        ex::fixed_size_simd<float,N> programIndex;
        programIndex.copy_from(&arange[0],ex::element_aligned_tag());

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i+=N)
            {
                ex::fixed_size_simd<float,N> x (x0 + (i + programIndex) * dx);
                ex::fixed_size_simd<float,N> y (y0 + j * dy);

                ex::fixed_size_simd_mask<float,N> active = x < ex::fixed_size_simd<float,N>(width);

                
                int base_index = (j * width + i);
                ex::fixed_size_simd<float,N> result = mandel<N>(active, x, y, maxIters);

                ex::fixed_size_simd<float,N> prev_data;
                prev_data.copy_from(output + base_index,ex::element_aligned_tag());

                ex::where(!active,result) = prev_data;
                result.copy_to(output + base_index,ex::element_aligned_tag());
            }
        }
    }

} // namespace mysimd


// run simd version of mandelbrot benchmark for a specific arch
template <int N, class bencher_t>
void run_arch(
    bencher_t& bencher,
    float x0,
    float y0,
    float x1,
    float y1,
    int width,
    int height,
    int maxIters,
    std::vector<int>& buffer)
{
    std::fill(buffer.begin(), buffer.end(), 0);
    auto stats = bencher([&]()
                         { mysimd::mandelbrot<N>(x0, y0, x1, y1, width, height, maxIters, buffer.data()); });

    const float scalar_min = stats.min().count();

    std::cout << '\n'
              << "mysimd N = " << N << " " << stats << '\n';
              
    auto filename = std::string("mandelbrot_") + std::to_string(N)+ std::string(".ppm");
    writePPM(filename.c_str(), width, height, buffer.data());

}

// run simd version of mandelbrot benchmark for a list
// of archs
template <int... N>
struct run_archlist
{
    template <class bencher_t>
    static void run(
        bencher_t& bencher,
        float x0,
        float y0,
        float x1,
        float y1,
        int width,
        int height,
        int maxIters,
        std::vector<int>& buffer)
    {
        using expand_type = int[];
        expand_type { (run_arch<N>(bencher, x0, y0, x1, x1, width, height, maxIters, buffer), 0)... };

    }
};

namespace VCL{

template <typename _Tp,typename _Tpb,typename _Tpi>
    inline _Tpi mandel(const _Tpb& _active,
                                   const _Tp& c_re,
                                   const _Tp& c_im,
                                   int maxIters)
    {

        std::size_t N = _Tp().size();

        _Tp z_re = c_re;
        _Tp z_im = c_im;
        _Tpi vi(0);

        for (int i = 0; i < maxIters; ++i)
        {
            _Tpb active = _active & ((z_re * z_re + z_im * z_im) <= _Tp(4.f));
            if (!horizontal_max(active))
            {
                break;
            }

            _Tp new_re = z_re * z_re - z_im * z_im;
            _Tp new_im = 2.f * z_re * z_im;

            z_re = c_re + new_re;
            z_im = c_im + new_im;

            vi = select(active, vi + 1, vi);
        }
        return vi;
    }

template <typename _Tp,typename _Tpb,typename _Tpi>
    void mandelbrot(float x0,
                    float y0,
                    float x1,
                    float y1,
                    int width,
                    int height,
                    int maxIters,
                    int output[])
    {
        
        std::size_t N = _Tp().size();
        float dx = (x1 - x0) / width;
        float dy = (y1 - y0) / height;

        float arange[N];
        std::iota(&arange[0], &arange[N], 0.f);
        _Tp programIndex;
        programIndex.load(arange);
        

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i += N)
            {
                _Tp x(x0 + (i + programIndex) * dx);
                _Tp y(y0 + j * dy);

                auto active = x < _Tp(width);
                int base_index = (j * width + i);
                auto result = mandel<_Tp,_Tpb,_Tpi>(active, x, y, maxIters);

                _Tpi prev_data;
                prev_data.load(output + base_index);

                select(active,result,prev_data).store(output + base_index);
            }
        }
    }


}// end namespace VCL

namespace xsimd
{

    template <class arch>
    inline batch<int, arch> mandel(const batch_bool<float, arch>& _active,
                                   const batch<float, arch>& c_re,
                                   const batch<float, arch>& c_im,
                                   int maxIters)
    {
        using float_batch_type = batch<float, arch>;
        using int_batch_type = batch<int, arch>;

        constexpr std::size_t N = float_batch_type::size;

        float_batch_type z_re = c_re;
        float_batch_type z_im = c_im;
        int_batch_type vi(0);

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

            vi = select(bool_cast(active), vi + 1, vi);
        }
        return vi;
    }

    template <class arch>
    void mandelbrot(float x0,
                    float y0,
                    float x1,
                    float y1,
                    int width,
                    int height,
                    int maxIters,
                    int output[])
    {
        using float_batch_type = batch<float, arch>;
        using int_batch_type = batch<int, arch>;

        constexpr std::size_t N = float_batch_type::size;
        float dx = (x1 - x0) / width;
        float dy = (y1 - y0) / height;

        float arange[N];
        std::iota(&arange[0], &arange[N], 0.f);
        // float_batch_type programIndex(&arange[0], xsimd::aligned_mode());

        auto programIndex = float_batch_type::load(&arange[0], xsimd::aligned_mode());
        // std::iota(programIndex.begin(), programIndex.end(), 0.f);

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i += N)
            {
                float_batch_type x(x0 + (i + programIndex) * dx);
                float_batch_type y(y0 + j * dy);

                auto active = x < float_batch_type(width);

                int base_index = (j * width + i);
                auto result = mandel<arch>(active, x, y, maxIters);

                // implement masked store!
                // xsimd::store_aligned(result, output + base_index, active);
                int_batch_type prev_data = int_batch_type::load_unaligned(output + base_index);
                select(bool_cast(active), result, prev_data)
                    .store_aligned(output + base_index);
            }
        }
    }

} // namespace xsimd

int main()
{
    using namespace std::chrono;

    const unsigned int width = 1024;
    const unsigned int height = 768;
    const float x0 = -2;
    const float x1 = 1;
    const float y0 = -1;
    const float y1 = 1;
    const int maxIters = 256;

    std::vector<int> buf(width * height);

    auto bencher = pico_bench::Benchmarker<nanoseconds> { 64, seconds { 10 } };

    std::cout << "starting benchmarks (results in 'ns')... " << '\n';

    // scalar run ///////////////////////////////////////////////////////////////

    std::fill(buf.begin(), buf.end(), 0);

    auto stats_scalar = bencher([&]()
                                { scalar::mandelbrot(x0, y0, x1, y1, width, height, maxIters, buf.data()); });

    const float scalar_min = stats_scalar.min().count();

    std::cout << '\n'
              << "scalar " << stats_scalar << '\n';


    // mysimd run ////////////////////////////////////////////////////////////////
    run_archlist<4,8>::run(bencher, x0, y0, x1, y1, width, height, maxIters, buf);

    // VCL run ////////////////////////////////////////////////////////////////
        //VCL n=4 ////////////////////////////////////////////////////////////////
    // std::fill(buf.begin(), buf.end(), 0);

    // auto stats_VCL4 = bencher([&]()
    //                             { VCL::mandelbrot<Vec4f,Vec4fb,Vec4i>(x0, y0, x1, y1, width, height, maxIters, buf.data()); });

    // const float VCL4_min = stats_VCL4.min().count();

    // std::cout << '\n'
    //           << "VCL N = 4 " << stats_VCL4 << '\n';
    //     //VCL n=8 ////////////////////////////////////////////////////////////////
    // auto stats_VCL8 = bencher([&]()
    //                             { VCL::mandelbrot<Vec8f,Vec8fb,Vec8i>(x0, y0, x1, y1, width, height, maxIters, buf.data()); });

    // const float VCL8_min = stats_VCL8.min().count();

    // std::cout << '\n'
    //           << "VCL N = 8 " << stats_VCL8 << '\n';

    // xsimd run ////////////////////////////////////////////////////////////////
        // xsimd SSE4.2 ////////////////////////////////////////////////////////////////
    std::fill(buf.begin(), buf.end(), 0);

    auto stats_xsimd_sse4_2 = bencher([&]()
                                { xsimd::mandelbrot<xsimd::sse4_2>(x0, y0, x1, y1, width, height, maxIters, buf.data()); });

    const float xsimd_sse4_2_min = stats_xsimd_sse4_2.min().count();

    std::cout << '\n'
              << "xsimd sse4.2 " << stats_xsimd_sse4_2 << '\n';
        // xsimd AVX2 ////////////////////////////////////////////////////////////////
    std::fill(buf.begin(), buf.end(), 0);

    auto stats_xsimd_avx_2 = bencher([&]()
                                { xsimd::mandelbrot<xsimd::avx>(x0, y0, x1, y1, width, height, maxIters, buf.data()); });

    const float xsimd_avx_2_min = stats_xsimd_avx_2.min().count();

    std::cout << '\n'
              << "xsimd avx2 " << stats_xsimd_avx_2 << '\n';
    return 0;
}
