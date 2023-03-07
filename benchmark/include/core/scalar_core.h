#include <algorithm>
#include <concepts>
#include <math.h>

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct scalar_t
{
  using Vec = Tp;
  using Mask = bool;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using scalar_t_v = typename scalar_t<Tp>::Vec;

template <typename Tp>
using scalar_t_m = typename scalar_t<Tp>::Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_SCALAR_VEC_NATIVE = std::same_as<Vec, scalar_t_v<Tp>>;

template <typename Vec, typename Tp>
concept IS_SCALAR_VEC = IS_SCALAR_VEC_NATIVE<Vec, Tp>;

template <typename Vec, typename Tp>
concept IS_SCALAR_MASK_NATIVE = std::same_as<Vec, scalar_t_m<Tp>>;

template <typename Vec, typename Tp>
concept IS_SCALAR_MASK = IS_SCALAR_MASK_NATIVE<Vec, Tp>;

template <typename Mask, typename Tp>
concept IS_SCALAR_TYPE = IS_SCALAR_MASK<Mask, Tp> || IS_SCALAR_VEC<Mask, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_SCALAR_TYPE<Up, Tp>
  {
    u = *mem;
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_SCALAR_TYPE<Up, Tp>
  {
    u = *mem;
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_SCALAR_TYPE<Up, Tp>
  {
    *mem = u;
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_SCALAR_TYPE<Up, Tp>
  {
    *mem = u;
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  std::size_t Len()
    requires IS_SCALAR_TYPE<Up, Tp>
  {
    return 1;
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_SCALAR_VEC<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_SCALAR_MASK<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, std::size_t i)
    requires IS_SCALAR_TYPE<Up, Tp>
  {
    return u;
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto Select(const Mask &m, const Vec &T, const Vec &F)
    requires IS_SCALAR_MASK<Mask, Tp> && IS_SCALAR_VEC<Vec, Tp>
  {
    return m ? T : F;
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const scalar_t_v<Tp> &u)
  {
    return abs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const scalar_t_v<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const scalar_t_v<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const scalar_t_v<Tp> &u)
  {
    return sqrt(u);
  }
  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const scalar_t_v<Tp> &u, const scalar_t_v<Tp> &v)
  {
    return max(u, v);
  }

  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const scalar_t_m<Tp> &u)
  {
    return u ? false : true;
  }

  template <typename Tp>
  bool Any(const scalar_t_m<Tp> &u)
  {
    return u ? true : false;
  }

  template <typename Tp>
  bool All(const scalar_t_m<Tp> &u)
  {
    return u ? true : false;
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const scalar_t_m<Tp> &m1, const scalar_t_m<Tp> &m2)
  {
    return (m1 & m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const scalar_t_m<Tp> &m1, const scalar_t_m<Tp> &m2)
  {
    return (m1 | m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const scalar_t_m<Tp> &m)
  {
    return (!m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_SCALAR_VEC<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_SCALAR_VEC<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }

} // namespace details