#include "MIPP/src/mipp.h"

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct mipp_t
{
  using Native_Vec = mipp::Reg<Tp>;
  using Native_Mask = mipp::Msk<mipp::N<Tp>()>;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using mipp_t_v_native = mipp_t<Tp>::Native_Vec;

template <typename Tp>
using mipp_t_m_native = mipp_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_MIPP_VEC_NATIVE = std::same_as<Vec, mipp_t_v_native<Tp>>;

template <typename Vec, typename Tp>
concept IS_MIPP_MASK_NATIVE = std::same_as<Vec, mipp_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_MIPP_NATIVE_TYPE = IS_MIPP_VEC_NATIVE<Up, Tp> || IS_MIPP_MASK_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_MIPP_NATIVE_TYPE<Up, Tp>
  {
    u.load(mem);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_MIPP_NATIVE_TYPE<Up, Tp>
  {
    u.loadu(mem);
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_MIPP_NATIVE_TYPE<Up, Tp>
  {
    u.store(mem);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_MIPP_NATIVE_TYPE<Up, Tp>
  {
    u.storeu(mem);
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  size_t Len()
    requires IS_MIPP_NATIVE_TYPE<Up, Tp>
  {
    return mipp::N<Tp>();
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_MIPP_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_MIPP_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, size_t i)
    requires IS_MIPP_NATIVE_TYPE<Up, Tp>
  {
    return u[i];
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto Select(const Mask &m, const Vec &T, const Vec &F)
    requires IS_MIPP_MASK_NATIVE<Mask, Tp> && IS_MIPP_VEC_NATIVE<Vec, Tp>
  {
    return mipp::mask<Tp, mipp::add>(m, F, T, BroadCast<Vec, Tp>(Tp(0)));
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const mipp_t_v_native<Tp> &u)
  {
    return mipp::abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const mipp_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return mipp::exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const mipp_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return mipp::log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const mipp_t_v_native<Tp> &u)
  {
    return mipp::sqrt(u);
  }

  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const mipp_t_v_native<Tp> &u, const mipp_t_v_native<Tp> &v)
  {
    return mipp::max(u, v);
  }

  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const mipp_t_m_native<Tp> &u)
  {
    return mipp::testz(u);
  }

  template <typename Tp>
  bool Any(const mipp_t_m_native<Tp> &u)
  {
    return !mipp::testz(u);
  }

  template <typename Tp>
  bool All(const mipp_t_m_native<Tp> &u)
  {
    for (std::size_t i = 0; i < Len<mipp_t_m_native<Tp>, Tp>(); ++i)
    {
      if (!u[i])
        return false;
    }
    return true;
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const mipp_t_m_native<Tp> &m1, const mipp_t_m_native<Tp> &m2)
  {
    return mipp::andb(m1, m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const mipp_t_m_native<Tp> &m1, const mipp_t_m_native<Tp> &m2)
  {
    return mipp::orb(m1, m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const mipp_t_m_native<Tp> &m)
  {
    return mipp::notb(m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_MIPP_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return mipp::lshift(l, r);
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_MIPP_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return mipp::rshift(l, r);
  }

} // namespace details