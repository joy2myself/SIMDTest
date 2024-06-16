/*   The functions of this file is to obtain the value of simd type length in compile-time,
  such as being used by computing non-type template parameters.
*/

#pragma once

#include <array>
#include <experimental/simd>
#include <type_traits>

#include "MIPP/src/mipp.h"
#include "tsimd/tsimd/tsimd.h"
#include "version2/vectorclass.h"
#include "xsimd/include/xsimd/xsimd.hpp"
#include "eve/algo.hpp"
#include "eve/logical.hpp"
#include "eve/module/core.hpp"
#include "eve/module/core/regular/if_else.hpp"
#include "eve/wide.hpp"
#include "hwy/contrib/math/math-inl.h"
#include "hwy/highway.h"
// #include "nsimd/nsimd-all.hpp"
// #include <Vc/Vc>

namespace ex = std::experimental::parallelism_v2;
namespace hn = hwy::HWY_NAMESPACE;

namespace utility
{
  /////////////////////////STATIC SIZE/////////////////////////////

  template <typename Tp, auto N>
  constexpr auto arraySize(const std::array<Tp, N> &) noexcept
  {
    return N;
  }

  //////////////////////////////VCL////////////////////////////////

  template <typename Tp>
  constexpr auto GetVecType()
  {
#if INSTRSET >= 2 && INSTRSET <= 6

    if constexpr (std::same_as<Tp, float>)
    {
      return Vec4f{};
    }

    else if constexpr (std::same_as<Tp, double>)
    {
      return Vec2d{};
    }

    else if constexpr (std::same_as<Tp, int8_t>)
    {
      return Vec16c{};
    }

    else if constexpr (std::same_as<Tp, uint8_t>)
    {
      return Vec16uc{};
    }

    else if constexpr (std::same_as<Tp, int16_t>)
    {
      return Vec8s{};
    }

    else if constexpr (std::same_as<Tp, uint16_t>)
    {
      return Vec8us{};
    }

    else if constexpr (std::same_as<Tp, int32_t>)
    {
      return Vec4i{};
    }

    else if constexpr (std::same_as<Tp, uint32_t>)
    {
      return Vec4ui{};
    }

    else if constexpr (std::same_as<Tp, int64_t>)
    {
      return Vec2q{};
    }

    else if constexpr (std::same_as<Tp, uint64_t>)
    {
      return Vec2uq{};
    }

#elif INSTRSET >= 7 && INSTRSET <= 8

    if constexpr (std::same_as<Tp, float>)
    {
      return Vec8f{};
    }

    else if constexpr (std::same_as<Tp, double>)
    {
      return Vec4d{};
    }

    else if constexpr (std::same_as<Tp, int8_t>)
    {
      return Vec32c{};
    }

    else if constexpr (std::same_as<Tp, uint8_t>)
    {
      return Vec32uc{};
    }

    else if constexpr (std::same_as<Tp, int16_t>)
    {
      return Vec16s{};
    }

    else if constexpr (std::same_as<Tp, uint16_t>)
    {
      return Vec16us{};
    }

    else if constexpr (std::same_as<Tp, int32_t>)
    {
      return Vec8i{};
    }

    else if constexpr (std::same_as<Tp, uint32_t>)
    {
      return Vec8ui{};
    }

    else if constexpr (std::same_as<Tp, int64_t>)
    {
      return Vec4q{};
    }

    else if constexpr (std::same_as<Tp, uint64_t>)
    {
      return Vec4uq{};
    }

#elif INSTRSET >= 9

    if constexpr (std::same_as<Tp, float>)
    {
      return Vec16f{};
    }

    else if constexpr (std::same_as<Tp, double>)
    {
      return Vec8d{};
    }

    else if constexpr (std::same_as<Tp, int8_t>)
    {
      return Vec64c{};
    }

    else if constexpr (std::same_as<Tp, uint8_t>)
    {
      return Vec64uc{};
    }

    else if constexpr (std::same_as<Tp, int16_t>)
    {
      return Vec32s{};
    }

    else if constexpr (std::same_as<Tp, uint16_t>)
    {
      return Vec32us{};
    }

    else if constexpr (std::same_as<Tp, int32_t>)
    {
      return Vec16i{};
    }

    else if constexpr (std::same_as<Tp, uint32_t>)
    {
      return Vec16ui{};
    }

    else if constexpr (std::same_as<Tp, int64_t>)
    {
      return Vec8q{};
    }

    else if constexpr (std::same_as<Tp, uint64_t>)
    {
      return Vec8uq{};
    }

#endif
  }

  template <typename Tp>
  constexpr auto GetMaskType()
  {
#if INSTRSET >= 2 && INSTRSET <= 6

    if constexpr (std::same_as<Tp, float>)
    {
      return Vec4fb{};
    }

    else if constexpr (std::same_as<Tp, double>)
    {
      return Vec2db{};
    }

    else if constexpr (std::same_as<Tp, int8_t>)
    {
      return Vec16cb{};
    }

    else if constexpr (std::same_as<Tp, uint8_t>)
    {
      return Vec16cb{};
    }

    else if constexpr (std::same_as<Tp, int16_t>)
    {
      return Vec8sb{};
    }

    else if constexpr (std::same_as<Tp, uint16_t>)
    {
      return Vec8sb{};
    }

    else if constexpr (std::same_as<Tp, int32_t>)
    {
      return Vec4ib{};
    }

    else if constexpr (std::same_as<Tp, uint32_t>)
    {
      return Vec4ib{};
    }

    else if constexpr (std::same_as<Tp, int64_t>)
    {
      return Vec2qb{};
    }

    else if constexpr (std::same_as<Tp, uint64_t>)
    {
      return Vec2qb{};
    }

#elif INSTRSET >= 7 && INSTRSET <= 8

    if constexpr (std::same_as<Tp, float>)
    {
      return Vec8fb{};
    }

    else if constexpr (std::same_as<Tp, double>)
    {
      return Vec4db{};
    }

    else if constexpr (std::same_as<Tp, int8_t>)
    {
      return Vec32cb{};
    }

    else if constexpr (std::same_as<Tp, uint8_t>)
    {
      return Vec32cb{};
    }

    else if constexpr (std::same_as<Tp, int16_t>)
    {
      return Vec16sb{};
    }

    else if constexpr (std::same_as<Tp, uint16_t>)
    {
      return Vec16sb{};
    }

    else if constexpr (std::same_as<Tp, int32_t>)
    {
      return Vec8ib{};
    }

    else if constexpr (std::same_as<Tp, uint32_t>)
    {
      return Vec8ib{};
    }

    else if constexpr (std::same_as<Tp, int64_t>)
    {
      return Vec4qb{};
    }

    else if constexpr (std::same_as<Tp, uint64_t>)
    {
      return Vec4qb{};
    }

#elif INSTRSET >= 9

    if constexpr (std::same_as<Tp, float>)
    {
      return Vec16fb{};
    }

    else if constexpr (std::same_as<Tp, double>)
    {
      return Vec8db{};
    }

    else if constexpr (std::same_as<Tp, int8_t>)
    {
      return Vec64cb{};
    }

    else if constexpr (std::same_as<Tp, uint8_t>)
    {
      return Vec64cb{};
    }

    else if constexpr (std::same_as<Tp, int16_t>)
    {
      return Vec32sb{};
    }

    else if constexpr (std::same_as<Tp, uint16_t>)
    {
      return Vec32sb{};
    }

    else if constexpr (std::same_as<Tp, int32_t>)
    {
      return Vec16ib{};
    }

    else if constexpr (std::same_as<Tp, uint32_t>)
    {
      return Vec16ib{};
    }

    else if constexpr (std::same_as<Tp, int64_t>)
    {
      return Vec8qb{};
    }

    else if constexpr (std::same_as<Tp, uint64_t>)
    {
      return Vec8qb{};
    }

#endif
  }

} // namespace utilty
