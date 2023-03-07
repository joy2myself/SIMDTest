#include "xsimd/include/xsimd/xsimd.hpp"

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct xsimd_t
{
  using Native_Vec = xsimd::batch<Tp>;
  using Native_Mask = xsimd::batch_bool<Tp>;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using xsimd_t_v_native = xsimd_t<Tp>::Native_Vec;

template <typename Tp>
using xsimd_t_m_native = xsimd_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_XSIMD_VEC_NATIVE = std::same_as<Vec, xsimd_t_v_native<Tp>>;

template <typename Mask, typename Tp>
concept IS_XSIMD_MASK_NATIVE = std::same_as<Mask, xsimd_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_XSIMD_NATIVE_TYPE = IS_XSIMD_VEC_NATIVE<Up, Tp> || IS_XSIMD_MASK_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_XSIMD_NATIVE_TYPE<Up, Tp>
  {
    u = xsimd::load_aligned(mem);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_XSIMD_NATIVE_TYPE<Up, Tp>
  {
    u = xsimd::load_unaligned(mem);
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_XSIMD_NATIVE_TYPE<Up, Tp>
  {
    u.store_aligned(mem);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_XSIMD_NATIVE_TYPE<Up, Tp>
  {
    u.store_unaligned(mem);
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  size_t Len()
    requires IS_XSIMD_NATIVE_TYPE<Up, Tp>
  {
    return Up::size;
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_XSIMD_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_XSIMD_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, size_t i)
    requires IS_XSIMD_NATIVE_TYPE<Up, Tp>
  {
    return u.get(i);
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto
  Select(const Mask &m, const Vec &T, const Vec &F)
    requires IS_XSIMD_MASK_NATIVE<Mask, Tp> && IS_XSIMD_VEC_NATIVE<Vec, Tp>
  {
    return xsimd::select(m, T, F);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const xsimd_t_v_native<Tp> &u)
  {
    return xsimd::abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const xsimd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return xsimd::exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const xsimd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return xsimd::log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const xsimd_t_v_native<Tp> &u)
  {
    return xsimd::sqrt(u);
  }

  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const xsimd_t_v_native<Tp> &u, const xsimd_t_v_native<Tp> &v)
  {
    return xsimd::max(u, v);
  }

  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const xsimd_t_m_native<Tp> &u)
  {
    return xsimd::none(u);
  }

  template <typename Tp>
  bool Any(const xsimd_t_m_native<Tp> &u)
  {
    return !xsimd::none(u);
  }

  template <typename Tp>
  bool All(const xsimd_t_m_native<Tp> &u)
  {
    return xsimd::all(u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const xsimd_t_m_native<Tp> &m1, const xsimd_t_m_native<Tp> &m2)
  {
    return (m1 & m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const xsimd_t_m_native<Tp> &m1, const xsimd_t_m_native<Tp> &m2)
  {
    return (m1 | m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const xsimd_t_m_native<Tp> &m)
  {
    return (!m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_XSIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_XSIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }

} // namespace details