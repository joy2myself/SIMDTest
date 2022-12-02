#include "eve/algo.hpp"
#include "eve/logical.hpp"
#include "eve/module/core.hpp"
#include "eve/module/core/regular/if_else.hpp"
#include "eve/module/math.hpp"
#include "eve/wide.hpp"

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template<typename Tp> struct eve_t
{
  using Native_Vec = eve::wide<Tp>;
  using Native_Mask = eve::logical<eve::wide<Tp>>;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template<typename Tp> using eve_t_v_native = eve_t<Tp>::Native_Vec;

template<typename Tp> using eve_t_m_native = eve_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template<typename Vec, typename Tp> concept IS_EVE_VEC_NATIVE = std::same_as<Vec, eve_t_v_native<Tp>>;

template<typename Mask, typename Tp> concept IS_EVE_MASK_NATIVE = std::same_as<Mask, eve_t_m_native<Tp>>;

template<typename Up, typename Tp> concept IS_EVE_NATIVE_TYPE = IS_EVE_VEC_NATIVE<Up, Tp> || IS_EVE_MASK_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template<typename Up, typename Tp> void Load_Aligned(Up &u, const Tp *mem) requires IS_EVE_NATIVE_TYPE<Up, Tp>
  {
    u = eve::load(eve::as_aligned(mem, eve::fixed<Up::cardinal_type::value>{}), eve::fixed<Up::cardinal_type::value>{});
  }

  template<typename Up, typename Tp> void Load_Unaligned(Up &u, const Tp *mem) requires IS_EVE_NATIVE_TYPE<Up, Tp>
  {
    u = eve::load(mem, eve::lane<Up::cardinal_type::value>);
  }

  /////////////////////////////STORE/////////////////////////////////

  template<typename Up, typename Tp> void Store_Aligned(Up &u, Tp *mem) requires IS_EVE_NATIVE_TYPE<Up, Tp>
  {
    eve::store(u, eve::as_aligned(mem, eve::fixed<Up::cardinal_type::value>{}));
  }

  template<typename Up, typename Tp> void Store_Unaligned(Up &u, Tp *mem) requires IS_EVE_NATIVE_TYPE<Up, Tp>
  {
    eve::store(u, mem);
  }

  /////////////////////////////LEN////////////////////////////////////

  template<typename Up, typename Tp> std::size_t Len() requires IS_EVE_NATIVE_TYPE<Up, Tp>
  {
    return Up::cardinal_type::value;
  }

  /////////////////////////////BroadCast//////////////////////////////

  template<typename Vec, typename Tp> auto BroadCast(Tp elem) requires IS_EVE_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template<typename Mask, typename Tp> auto BroadCast(Tp elem) requires IS_EVE_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template<typename Up, typename Tp> Tp Get(const Up &u, std::size_t i) requires IS_EVE_NATIVE_TYPE<Up, Tp>
  {
    return u.get(i);
  }

  /////////////////////////////SELECT/////////////////////////////////

  template<typename Mask, typename Vec, typename Tp>
  auto Select(const Mask &m, const Vec &T, const Vec &F) requires IS_EVE_MASK_NATIVE<Mask, Tp> && IS_EVE_VEC_NATIVE<Vec, Tp>
  {
    return eve::if_else(m, T, F);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template<typename Tp> auto Fabs(const eve_t_v_native<Tp> &u)
  {
    return eve::abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template<typename Tp> auto Exp(const eve_t_v_native<Tp> &u) requires std::is_floating_point_v<Tp>
  {
    return eve::exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template<typename Tp> auto Log(const eve_t_v_native<Tp> &u) requires std::is_floating_point_v<Tp>
  {
    return eve::log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template<typename Tp> auto Sqrt(const eve_t_v_native<Tp> &u)
  {
    return eve::sqrt(u);
  }
  /////////////////////////////MAX//////////////////////////////////

  template<typename Tp> auto Max(const eve_t_v_native<Tp> &u, const eve_t_v_native<Tp> &v)
  {
    return eve::max(u, v);
  }
  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template<typename Tp> bool None(const eve_t_m_native<Tp> &u)
  {
    return eve::none(u);
  }

  template<typename Tp> bool Any(const eve_t_m_native<Tp> &u)
  {
    return eve::any(u);
  }

  template<typename Tp> bool All(const eve_t_m_native<Tp> &u)
  {
    return eve::all(u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template<typename Tp> auto And(const eve_t_m_native<Tp> &m1, const eve_t_m_native<Tp> &m2)
  {
    return eve::logical_and(m1, m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template<typename Tp> auto Or(const eve_t_m_native<Tp> &m1, const eve_t_m_native<Tp> &m2)
  {
    return eve::logical_or(m1, m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template<typename Tp> auto Not(const eve_t_m_native<Tp> &m)
  {
    return eve::logical_not(m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template<typename Vec, typename Tp> Vec LeftShift(const Vec &l, const Tp r) requires IS_EVE_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template<typename Vec, typename Tp> Vec RightShift(const Vec &l, const Tp r) requires IS_EVE_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }

} // namespace details