#include "nsimd/nsimd-all.hpp"

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct nsimd_t
{
  using Native_Vec = nsimd::pack<Tp>;
  using Native_Mask = nsimd::packl<Tp>;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using nsimd_t_v_native = nsimd_t<Tp>::Native_Vec;

template <typename Tp>
using nsimd_t_m_native = nsimd_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_NSIMD_VEC_NATIVE = std::same_as<Vec, nsimd_t_v_native<Tp>>;

template <typename Mask, typename Tp>
concept IS_NSIMD_MASK_NATIVE = std::same_as<Mask, nsimd_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_NSIMD_NATIVE_TYPE = IS_NSIMD_VEC_NATIVE<Up, Tp> || IS_NSIMD_MASK_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_NSIMD_VEC_NATIVE<Up, Tp>
  {
    u = nsimd::loada<Up>(mem);
  }

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_NSIMD_MASK_NATIVE<Up, Tp>
  {
    u = nsimd::loadla<Up>(mem);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_NSIMD_VEC_NATIVE<Up, Tp>
  {
    u = nsimd::loadu<Up>(mem);
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_NSIMD_MASK_NATIVE<Up, Tp>
  {
    u = nsimd::loadlu<Up>(mem);
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_NSIMD_VEC_NATIVE<Up, Tp>
  {
    nsimd::storea(mem, u);
  }

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_NSIMD_MASK_NATIVE<Up, Tp>
  {
    nsimd::storela(mem, u);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_NSIMD_VEC_NATIVE<Up, Tp>
  {
    nsimd::storeu(mem, u);
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_NSIMD_MASK_NATIVE<Up, Tp>
  {
    nsimd::storelu(mem, u);
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  size_t Len()
    requires IS_NSIMD_NATIVE_TYPE<Up, Tp>
  {
    return nsimd::len<Up>();
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_NSIMD_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_NSIMD_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, size_t i)
    requires IS_NSIMD_NATIVE_TYPE<Up, Tp>
  {
    Up tmp(u);
    Tp arr[Len<Up, Tp>()]{0};
    Store_Aligned<Up, Tp>(tmp, arr);
    return arr[i];
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto
  Select(const Mask &m, const Vec &T, const Vec &F)
    requires IS_NSIMD_MASK_NATIVE<Mask, Tp> && IS_NSIMD_VEC_NATIVE<Vec, Tp>
  {
    return nsimd::if_else(m, T, F);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  // TODO: fix math function ld error

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const nsimd_t_v_native<Tp> &u)
  {
    return nsimd::abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const nsimd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return nsimd::exp_u10(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const nsimd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return nsimd::log_u10(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const nsimd_t_v_native<Tp> &u)
  {
    return nsimd::sqrt(u);
  }

  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const nsimd_t_v_native<Tp> &u, const nsimd_t_v_native<Tp> &v)
  {
    return nsimd::max(u, v);
  }

  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const nsimd_t_m_native<Tp> &u)
  {
    return !nsimd::any(u);
  }

  template <typename Tp>
  bool Any(const nsimd_t_m_native<Tp> &u)
  {
    return nsimd::any(u);
  }

  template <typename Tp>
  bool All(const nsimd_t_m_native<Tp> &u)
  {
    return nsimd::all(u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  // TODO: FIX

  template <typename Tp>
  auto And(nsimd_t_m_native<Tp> m1, nsimd_t_m_native<Tp> m2) /////// & operation
  {
    Tp arr1[Len<nsimd_t_m_native<Tp>, Tp>()]{};
    Tp arr2[Len<nsimd_t_m_native<Tp>, Tp>()]{};
    Tp result[Len<nsimd_t_m_native<Tp>, Tp>()]{};

    nsimd_t_m_native<Tp> tmp;

    Store_Aligned<nsimd_t_m_native<Tp>, Tp>(m1, arr1);
    Store_Aligned<nsimd_t_m_native<Tp>, Tp>(m2, arr2);

    for (std::size_t i = 0; i < Len<nsimd_t_m_native<Tp>, Tp>(); ++i)
    {
      result[i] = arr1[i] && arr2[i];
    }

    Load_Aligned<nsimd_t_m_native<Tp>, Tp>(tmp, result);
    return tmp;
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(nsimd_t_m_native<Tp> m1, nsimd_t_m_native<Tp> m2) /////// :TODO nsimd has no | for mask
  {
    return m1 || m2;
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(nsimd_t_m_native<Tp> m) /////// ! operation
  {
    Tp arr[Len<nsimd_t_m_native<Tp>, Tp>()]{};
    Tp result[Len<nsimd_t_m_native<Tp>, Tp>()]{};

    nsimd_t_m_native<Tp> tmp;
    Store_Aligned<nsimd_t_m_native<Tp>, Tp>(m, arr);

    for (std::size_t i = 0; i < Len<nsimd_t_m_native<Tp>, Tp>(); ++i)
    {
      result[i] = !arr[i];
    }

    Load_Aligned<nsimd_t_m_native<Tp>, Tp>(tmp, result);
    return tmp;
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_NSIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_NSIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }

} // namespace details