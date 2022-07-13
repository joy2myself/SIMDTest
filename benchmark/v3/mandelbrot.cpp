#include <benchmark/benchmark.h>
#include <iostream>
#include <numeric>
#include <experimental/simd>
#include "../../simd_libraries/version2/vectorclass.h"
#include "../../simd_libraries/xsimd/include/xsimd/xsimd.hpp"
//#include "../../simd_libraries/libsimdpp/simdpp/simd.h"

namespace xs = xsimd;
namespace ex = std::experimental::parallelism_v2;
using namespace std;

const unsigned int _width = 1024;
const unsigned int _height = 768;
const float x_0 = -2;
const float x_1 = 1;
const float y_0 = -1;
const float y_1 = 1;
const int _maxIters = 256;
std::vector<int> _buf(_width * _height);



namespace _scalar_
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


namespace _VCL_{

#include "../../simd_libraries/version2/vectorclass.h"
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


namespace _mysimd_
{

template<int N>
    inline ex::fixed_size_simd<int,N> mandel(const ex::fixed_size_simd_mask<float,N>& _active,
                            const ex::fixed_size_simd<float,N>& c_re, 
                            const ex::fixed_size_simd<float,N>& c_im, 
                            int maxIters)
    {
        ex::fixed_size_simd<float,N> z_re = c_re;
        ex::fixed_size_simd<float,N> z_im = c_im;
        ex::fixed_size_simd<int,N> vi(0);

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
                ex::fixed_size_simd<int,N> result = mandel<N>(active, x, y, maxIters);

                ex::fixed_size_simd<int,N> prev_data;
                prev_data.copy_from(output + base_index,ex::element_aligned_tag());

                ex::where(!active,result) = prev_data;
                result.copy_to(output + base_index,ex::element_aligned_tag());
            }
        }
    }

} // namespace mysimd


namespace _xsimd_
{

    template <class arch>
    inline xs::batch<int, arch> mandel(const xs::batch_bool<float, arch>& _active,
                                   const xs::batch<float, arch>& c_re,
                                   const xs::batch<float, arch>& c_im,
                                   int maxIters)
    {
        using float_batch_type = xs::batch<float, arch>;
        using int_batch_type = xs::batch<int, arch>;

        float_batch_type z_re = c_re;
        float_batch_type z_im = c_im;
        int_batch_type vi(0);

        for (int i = 0; i < maxIters; ++i)
        {
            auto active = _active & ((z_re * z_re + z_im * z_im) <= float_batch_type(4.f));
            if (!xs::any(active))
            {
                break;
            }

            float_batch_type new_re = z_re * z_re - z_im * z_im;
            float_batch_type new_im = 2.f * z_re * z_im;

            z_re = c_re + new_re;
            z_im = c_im + new_im;

            vi = xs::select(bool_cast(active), vi + 1, vi);
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
        using float_batch_type = xs::batch<float, arch>;
        using int_batch_type = xs::batch<int, arch>;

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


namespace simdpp{


template<int N>
    inline ex::fixed_size_simd<int,N> mandel(const ex::fixed_size_simd_mask<float,N>& _active,
                            const ex::fixed_size_simd<float,N>& c_re, 
                            const ex::fixed_size_simd<float,N>& c_im, 
                            int maxIters)
    {
        ex::fixed_size_simd<float,N> z_re = c_re;
        ex::fixed_size_simd<float,N> z_im = c_im;
        ex::fixed_size_simd<int,N> vi(0);

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
                ex::fixed_size_simd<int,N> result = mandel<N>(active, x, y, maxIters);

                ex::fixed_size_simd<int,N> prev_data;
                prev_data.copy_from(output + base_index,ex::element_aligned_tag());

                ex::where(!active,result) = prev_data;
                result.copy_to(output + base_index,ex::element_aligned_tag());
            }
        }
    }

}// namespace simdpp


static void BM_Mandel_scalar(benchmark::State& state) {
    std::fill(_buf.begin(), _buf.end(), 0);
    for (auto _ : state)
        _scalar_::mandelbrot(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf.data());
}//scalar bench function

template <int N>
static void BM_Mandel_gcc_std_simd(benchmark::State& state) {
    std::fill(_buf.begin(), _buf.end(), 0);
    for (auto _ : state)
        _mysimd_::mandelbrot<N>(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf.data());
}// mysimd bench function

template <typename _Tp,typename _Tpb,typename _Tpi>
static void BM_Mandel_VCL(benchmark::State& state) {
    std::fill(_buf.begin(), _buf.end(), 0);
    for (auto _ : state)
        _VCL_::mandelbrot<_Tp,_Tpb,_Tpi>(x_0, y_0, x_1, y_1, _width, _height, _maxIters, _buf.data());
}//VCL bench function

template <typename arch>
static void BM_Mandel_xsimd(benchmark::State& state) {
    std::fill(_buf.begin(), _buf.end(), 0);
    for (auto _ : state)
        _xsimd_::mandelbrot<arch>(x_0, y_0, x_1, y_1, _width, _height, _maxIters,_buf.data());
}//XSIMD bench function



BENCHMARK(BM_Mandel_scalar);

BENCHMARK_TEMPLATE(BM_Mandel_gcc_std_simd, 4);
BENCHMARK_TEMPLATE(BM_Mandel_gcc_std_simd, 8);

BENCHMARK_TEMPLATE(BM_Mandel_VCL, Vec4f, Vec4fb, Vec4i);
BENCHMARK_TEMPLATE(BM_Mandel_VCL, Vec8f, Vec8fb, Vec8i);

BENCHMARK_TEMPLATE(BM_Mandel_xsimd, xs::sse4_2);
BENCHMARK_TEMPLATE(BM_Mandel_xsimd, xs::avx2);

BENCHMARK_MAIN();