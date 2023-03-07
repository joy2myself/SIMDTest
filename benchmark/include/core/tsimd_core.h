
#include "tsimd/tsimd/tsimd.h"
#include <algorithm>
#include <array>

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct tsimd_t
{
  using Native_Vec = tsimd::vpack<Tp>;
  using Native_Mask = tsimd::mask<Tp>;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using tsimd_t_v_native = tsimd_t<Tp>::Native_Vec;

template <typename Tp>
using tsimd_t_m_native = tsimd_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_TSIMD_VEC_NATIVE = std::same_as<Vec, tsimd_t_v_native<Tp>>;

template <typename Mask, typename Tp>
concept IS_TSIMD_MASK_NATIVE = std::same_as<Mask, tsimd_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_TSIMD_NATIVE_TYPE = IS_TSIMD_VEC_NATIVE<Up, Tp> || IS_TSIMD_MASK_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_TSIMD_NATIVE_TYPE<Up, Tp>
  {
    u = tsimd::load<Up>(mem);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_TSIMD_NATIVE_TYPE<Up, Tp>
  {
    // NOTE : tsimd has no unaligned load function, this is sequential implementation of tsimd unaligned load
    for (size_t i = 0; i < Up::static_size; ++i)
    {
      u[i] = *mem++;
    }
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_TSIMD_NATIVE_TYPE<Up, Tp>
  {
    tsimd::store(u, mem);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_TSIMD_NATIVE_TYPE<Up, Tp>
  {
    // NOTE : tsimd has no unaligned store function, this is sequential implementation of tsimd unaligned store
    for (size_t i = 0; i < Up::static_size; ++i)
    {
      *mem++ = u[i];
    }
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  size_t Len()
    requires IS_TSIMD_NATIVE_TYPE<Up, Tp>
  {
    return Up::static_size;
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_TSIMD_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_TSIMD_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, size_t i)
    requires IS_TSIMD_NATIVE_TYPE<Up, Tp>
  {
    return u[i];
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto
  Select(const Mask &m, const Vec &T, const Vec &F)
    requires IS_TSIMD_MASK_NATIVE<Mask, Tp> && IS_TSIMD_VEC_NATIVE<Vec, Tp>
  {
    return tsimd::select(m, T, F);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const tsimd_t_v_native<Tp> &u)
  {
    return tsimd::abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const tsimd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return tsimd::exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const tsimd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return tsimd::log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const tsimd_t_v_native<Tp> &u)
  {
    return tsimd::sqrt(u);
  }

  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const tsimd_t_v_native<Tp> &u, const tsimd_t_v_native<Tp> &v)
  {
    return tsimd::max(u, v);
  }

  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const tsimd_t_m_native<Tp> &u)
  {
    return tsimd::none(u);
  }

  template <typename Tp>
  bool Any(const tsimd_t_m_native<Tp> &u)
  {
    return tsimd::any(u);
  }

  template <typename Tp>
  bool All(const tsimd_t_m_native<Tp> &u)
  {
    return tsimd::all(u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const tsimd_t_m_native<Tp> &m1, const tsimd_t_m_native<Tp> &m2)
  {
    return tsimd_t_m_native<Tp>(m1 & m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const tsimd_t_m_native<Tp> &m1, const tsimd_t_m_native<Tp> &m2)
  {
    return tsimd_t_m_native<Tp>(m1 | m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const tsimd_t_m_native<Tp> &m)
  {
    return tsimd_t_m_native<Tp>(!m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_TSIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_TSIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }
} // namespace details