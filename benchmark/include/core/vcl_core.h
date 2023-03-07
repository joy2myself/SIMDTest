#include "../../utility.h"
#include "version2/vectorclass.h"
#include "version2/vectormath_exp.h"

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct vcl_t
{
  using Native_Vec = decltype(utility::GetVecType<Tp>());
  using Native_Mask = decltype(utility::GetMaskType<Tp>());
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using vcl_t_v_native = vcl_t<Tp>::Native_Vec;

template <typename Tp>
using vcl_t_m_native = vcl_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_VCL_VEC_NATIVE = std::same_as<Vec, vcl_t_v_native<Tp>>;

template <typename Mask, typename Tp>
concept IS_VCL_MASK_NATIVE = std::same_as<Mask, vcl_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_VCL_NATIVE_TYPE = IS_VCL_VEC_NATIVE<Up, Tp> || IS_VCL_MASK_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_VCL_NATIVE_TYPE<Up, Tp>
  {
    u.load_a(mem);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_VCL_NATIVE_TYPE<Up, Tp>
  {
    u.load(mem);
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_VCL_NATIVE_TYPE<Up, Tp>
  {
    u.store_a(mem);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_VCL_NATIVE_TYPE<Up, Tp>
  {
    u.store(mem);
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  size_t Len()
    requires IS_VCL_NATIVE_TYPE<Up, Tp>
  {
    return Up{}.size();
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_VCL_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_VCL_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, size_t i)
    requires IS_VCL_NATIVE_TYPE<Up, Tp>
  {
    return u[i];
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto Select(const Mask &m, const Vec &T, const Vec &F)
    requires IS_VCL_MASK_NATIVE<Mask, Tp> && IS_VCL_VEC_NATIVE<Vec, Tp>
  {
    return select(m, T, F);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const vcl_t_v_native<Tp> &u)
  {
    return abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const vcl_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const vcl_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const vcl_t_v_native<Tp> &u)
  {
    return sqrt(u);
  }
  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const vcl_t_v_native<Tp> &u, const vcl_t_v_native<Tp> &v)
  {
    return max(u, v);
  }
  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const vcl_t_m_native<Tp> &u)
  {
    return !horizontal_or(u);
  }

  template <typename Tp>
  bool Any(const vcl_t_m_native<Tp> &u)
  {
    return horizontal_or(u);
  }

  template <typename Tp>
  bool All(const vcl_t_m_native<Tp> &u)
  {
    return horizontal_and(u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const vcl_t_m_native<Tp> &m1, const vcl_t_m_native<Tp> &m2)
  {
    return (m1 & m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const vcl_t_m_native<Tp> &m1, const vcl_t_m_native<Tp> &m2)
  {
    return (m1 | m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const vcl_t_m_native<Tp> &m)
  {
    return (!m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_VCL_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_VCL_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }

} // namespace details