#include "Vc/Vc"
#include "Vc/src/const.cpp"

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct vc_t
{
  using Native_Vec = Vc::Vector<Tp>;
  using Native_Mask = Vc::Mask<Tp>;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using vc_t_v_native = vc_t<Tp>::Native_Vec;

template <typename Tp>
using vc_t_m_native = vc_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_VC_VEC_NATIVE = std::same_as<Vec, vc_t_v_native<Tp>>;

template <typename Mask, typename Tp>
concept IS_VC_MASK_NATIVE = std::same_as<Mask, vc_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_VC_NATIVE_TYPE = IS_VC_MASK_NATIVE<Up, Tp> || IS_VC_VEC_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_VC_NATIVE_TYPE<Up, Tp>
  {
    u.load(mem, Vc::Aligned);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_VC_NATIVE_TYPE<Up, Tp>
  {
    u.load(mem, Vc::Unaligned);
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_VC_NATIVE_TYPE<Up, Tp>
  {
    u.store(mem, Vc::Aligned);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_VC_NATIVE_TYPE<Up, Tp>
  {
    u.store(mem, Vc::Unaligned);
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  std::size_t Len()
    requires IS_VC_NATIVE_TYPE<Up, Tp>
  {
    return Up::Size;
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_VC_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_VC_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, std::size_t i)
    requires IS_VC_NATIVE_TYPE<Up, Tp>
  {
    return u[i];
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto
  Select(const Mask &m, const Vec &T, Vec &F)
    requires IS_VC_MASK_NATIVE<Mask, Tp> && IS_VC_VEC_NATIVE<Vec, Tp>
  {
    Vc::where(m, F) = T;
    return F;
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const vc_t_v_native<Tp> &u)
  {
    return Vc::abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const vc_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return Vc::exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const vc_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return Vc::log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const vc_t_v_native<Tp> &u)
  {
    return Vc::sqrt(u);
  }

  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const vc_t_v_native<Tp> &u, const vc_t_v_native<Tp> &v)
  {
    return Vc::max(u, v);
  }
  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const vc_t_m_native<Tp> &u)
  {
    return Vc::none_of(u);
  }

  template <typename Tp>
  bool Any(const vc_t_m_native<Tp> &u)
  {
    return Vc::any_of(u);
  }

  template <typename Tp>
  bool All(const vc_t_m_native<Tp> &u)
  {
    return Vc::all_of(u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const vc_t_m_native<Tp> &m1, const vc_t_m_native<Tp> &m2)
  {
    return (m1 & m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const vc_t_m_native<Tp> &m1, const vc_t_m_native<Tp> &m2)
  {
    return (m1 | m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const vc_t_m_native<Tp> &m)
  {
    return (!m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_VC_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_VC_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }

} // namespace details