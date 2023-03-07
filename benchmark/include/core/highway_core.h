#include "hwy/contrib/math/math-inl.h"
#include "hwy/highway.h"

namespace hn = hwy::HWY_NAMESPACE;

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct highway_t
{
  using Native_Vec = decltype(hn::Zero(hn::ScalableTag<Tp>{}));
  using Native_Mask = decltype(hn::MaskFromVec(hn::Zero(hn::ScalableTag<Tp>{})));
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using highway_t_v_native = typename highway_t<Tp>::Native_Vec;

template <typename Tp>
using highway_t_m_native = typename highway_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_HIGHWAY_VEC_NATIVE = std::same_as<Vec, highway_t_v_native<Tp>>;

template <typename Mask, typename Tp>
concept IS_HIGHWAY_MASK_NATIVE = std::same_as<Mask, highway_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_HIGHWAY_NATIVE_TYPE = IS_HIGHWAY_VEC_NATIVE<Up, Tp> || IS_HIGHWAY_MASK_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_HIGHWAY_NATIVE_TYPE<Up, Tp>
  {
    u = hn::Load(hn::ScalableTag<Tp>{}, mem);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_HIGHWAY_NATIVE_TYPE<Up, Tp>
  {
    u = hn::LoadU(hn::ScalableTag<Tp>{}, mem);
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_HIGHWAY_NATIVE_TYPE<Up, Tp>
  {
    hn::Store(u, hn::ScalableTag<Tp>{}, mem);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_HIGHWAY_NATIVE_TYPE<Up, Tp>
  {
    hn::StoreU(u, hn::ScalableTag<Tp>{}, mem);
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  size_t Len()
    requires IS_HIGHWAY_NATIVE_TYPE<Up, Tp>
  {
    return hn::Lanes(hn::ScalableTag<Tp>{});
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_HIGHWAY_VEC_NATIVE<Vec, Tp>
  {
    return hn::Set(hn::ScalableTag<Tp>{}, elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_HIGHWAY_MASK_NATIVE<Mask, Tp>
  {
    highway_t_v_native<Tp> v;
    return static_cast<bool>(elem) ? hn::Eq(v, v) : hn::Ne(v, v);
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, size_t i)
    requires IS_HIGHWAY_NATIVE_TYPE<Up, Tp>
  {
    return hn::ExtractLane(u, i);
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto Select(const Mask &m, const Vec &T, const Vec &F)
    requires IS_HIGHWAY_VEC_NATIVE<Vec, Tp> &&
             IS_HIGHWAY_MASK_NATIVE<Mask, Tp>
  {
    return hn::IfThenElse(m, T, F);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const highway_t_v_native<Tp> &u)
  {
    return hn::Abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const highway_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return hn::Exp(hn::ScalableTag<Tp>{}, u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const highway_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return hn::Log(hn::ScalableTag<Tp>{}, u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const highway_t_v_native<Tp> &u)
  {
    return hn::Sqrt(u);
  }
  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const highway_t_v_native<Tp> &u, const highway_t_v_native<Tp> &v)
  {
    return hn::Max(u, v);
  }

  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const highway_t_m_native<Tp> &u)
  {
    return hn::AllFalse(hn::ScalableTag<Tp>{}, u);
  }

  template <typename Tp>
  bool Any(const highway_t_m_native<Tp> &u)
  {
    return !hn::AllFalse(hn::ScalableTag<Tp>{}, u);
  }

  template <typename Tp>
  bool All(const highway_t_m_native<Tp> &u)
  {
    return hn::AllTrue(hn::ScalableTag<Tp>{}, u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const highway_t_m_native<Tp> &m1, const highway_t_m_native<Tp> &m2)
  {
    return hn::And(m1, m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const highway_t_m_native<Tp> &m1, const highway_t_m_native<Tp> &m2)
  {
    return hn::Or(m1, m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const highway_t_m_native<Tp> &m)
  {
    return hn::Not(m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_HIGHWAY_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << hn::Set(hn::ScalableTag<Tp>{}, r);
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_HIGHWAY_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> hn::Set(hn::ScalableTag<Tp>{}, r);
  }

} // namespace details

#if defined(__riscv_vector)
template <typename Tp>
auto operator+(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Add(a, b);
}

template <typename Tp>
auto operator-(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Sub(a, b);
}

template <typename Tp>
auto operator*(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Mul(a, b);
}

template <typename Tp>
auto operator/(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Div(a, b);
}

template <typename Tp>
auto operator>(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Gt(a, b);
}

template <typename Tp>
auto operator<(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Lt(a, b);
}

template <typename Tp>
auto operator>=(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Ge(a, b);
}

template <typename Tp>
auto operator<=(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Le(a, b);
}

template <typename Tp>
auto operator==(highway_t_v_native<Tp> a, highway_t_v_native<Tp> b)
{
  return hn::Eq(a, b);
}

#endif