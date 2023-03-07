#include <experimental/simd>

namespace ex = std::experimental::parallelism_v2;

/////////////////////////////SIMD PROTOTYPE//////////////////////////////////

template <typename Tp>
struct std_simd_t
{
  using Native_Vec = ex::native_simd<Tp>;
  using Native_Mask = ex::native_simd_mask<Tp>;
};

/////////////////////////////TYPE ALIAS//////////////////////////////////////

template <typename Tp>
using std_simd_t_v_native = std_simd_t<Tp>::Native_Vec;

template <typename Tp>
using std_simd_t_m_native = std_simd_t<Tp>::Native_Mask;

/////////////////////////////CONCEPTS////////////////////////////////////////

template <typename Vec, typename Tp>
concept IS_STD_SIMD_VEC_NATIVE = std::same_as<Vec, std_simd_t_v_native<Tp>>;

template <typename Mask, typename Tp>
concept IS_STD_SIMD_MASK_NATIVE = std::same_as<Mask, std_simd_t_m_native<Tp>>;

template <typename Up, typename Tp>
concept IS_STD_SIMD_NATIVE_TYPE = IS_STD_SIMD_MASK_NATIVE<Up, Tp> || IS_STD_SIMD_VEC_NATIVE<Up, Tp>;

/////////////////////////////FUNCTIONS///////////////////////////////////////

namespace details
{
  /////////////////////////////LOAD//////////////////////////////////

  template <typename Up, typename Tp>
  void Load_Aligned(Up &u, const Tp *mem)
    requires IS_STD_SIMD_NATIVE_TYPE<Up, Tp>
  {
    u.copy_from(mem, ex::element_aligned_tag());
  }

  template <typename Up, typename Tp>
  void Load_Unaligned(Up &u, const Tp *mem)
    requires IS_STD_SIMD_NATIVE_TYPE<Up, Tp>
  {
    // TODO : fix unaligned load tag of std_simd
    u.copy_from(mem, ex::element_aligned_tag());
  }

  /////////////////////////////STORE/////////////////////////////////

  template <typename Up, typename Tp>
  void Store_Aligned(Up &u, Tp *mem)
    requires IS_STD_SIMD_NATIVE_TYPE<Up, Tp>
  {
    u.copy_to(mem, ex::element_aligned_tag());
  }

  template <typename Up, typename Tp>
  void Store_Unaligned(Up &u, Tp *mem)
    requires IS_STD_SIMD_NATIVE_TYPE<Up, Tp>
  {
    // TODO : fix unaligned store tag of std_simd
    u.copy_to(mem, ex::element_aligned_tag());
  }

  /////////////////////////////LEN////////////////////////////////////

  template <typename Up, typename Tp>
  std::size_t Len()
    requires IS_STD_SIMD_NATIVE_TYPE<Up, Tp>
  {
    return Up{}.size();
  }

  /////////////////////////////BroadCast//////////////////////////////

  template <typename Vec, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_STD_SIMD_VEC_NATIVE<Vec, Tp>
  {
    return Vec(elem);
  }

  template <typename Mask, typename Tp>
  auto BroadCast(Tp elem)
    requires IS_STD_SIMD_MASK_NATIVE<Mask, Tp>
  {
    return Mask(static_cast<bool>(elem));
  }

  /////////////////////////////GET////////////////////////////////////

  template <typename Up, typename Tp>
  Tp Get(const Up &u, std::size_t i)
    requires IS_STD_SIMD_NATIVE_TYPE<Up, Tp>
  {
    return u[i];
  }

  /////////////////////////////SELECT/////////////////////////////////

  template <typename Mask, typename Vec, typename Tp>
  auto
  Select(const Mask &m, const Vec &T, Vec &F)
    requires IS_STD_SIMD_MASK_NATIVE<Mask, Tp> && IS_STD_SIMD_VEC_NATIVE<Vec, Tp>
  {
    ex::where(m, F) = T;
    return F;
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  /////////////////////////////FABS/////////////////////////////////

  template <typename Tp>
  auto Fabs(const std_simd_t_v_native<Tp> &u)
  {
    return fabs(u);
  }

  /////////////////////////////EXP//////////////////////////////////

  template <typename Tp>
  auto Exp(const std_simd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return exp(u);
  }

  /////////////////////////////LOG//////////////////////////////////

  template <typename Tp>
  auto Log(const std_simd_t_v_native<Tp> &u)
    requires std::is_floating_point_v<Tp>
  {
    return log(u);
  }

  /////////////////////////////SQRT//////////////////////////////////

  template <typename Tp>
  auto Sqrt(const std_simd_t_v_native<Tp> &u)
  {
    return sqrt(u);
  }

  /////////////////////////////MAX//////////////////////////////////

  template <typename Tp>
  auto Max(const std_simd_t_v_native<Tp> &u, const std_simd_t_v_native<Tp> &v)
  {
    return ex::max(u, v);
  }

  /////////////////////////////REDUCTION///////////////////////////////////

  /////////////////////////////ANY//////////////////////////////////

  template <typename Tp>
  bool None(const std_simd_t_m_native<Tp> &u)
  {
    return ex::none_of(u);
  }

  template <typename Tp>
  bool Any(const std_simd_t_m_native<Tp> &u)
  {
    return ex::any_of(u);
  }

  template <typename Tp>
  bool All(const std_simd_t_m_native<Tp> &u)
  {
    return ex::all_of(u);
  }

  /////////////////////////////LOGIC///////////////////////////////////

  /////////////////////////////AND//////////////////////////////////

  template <typename Tp>
  auto And(const std_simd_t_m_native<Tp> &m1, const std_simd_t_m_native<Tp> &m2)
  {
    return (m1 & m2);
  }

  /////////////////////////////OR//////////////////////////////////

  template <typename Tp>
  auto Or(const std_simd_t_m_native<Tp> &m1, const std_simd_t_m_native<Tp> &m2)
  {
    return (m1 | m2);
  }

  /////////////////////////////NOT//////////////////////////////////

  template <typename Tp>
  auto Not(const std_simd_t_m_native<Tp> &m)
  {
    return (!m);
  }

  /////////////////////////////VEC ARITHMATIC/////////////////////////////////////

  ////////////////////////////////LEFT SHIFT//////////////////////////////////

  template <typename Vec, typename Tp>
  Vec LeftShift(const Vec &l, const Tp r)
    requires IS_STD_SIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l << r;
  }

  template <typename Vec, typename Tp>
  Vec RightShift(const Vec &l, const Tp r)
    requires IS_STD_SIMD_NATIVE_TYPE<Vec, Tp> && std::is_integral_v<Tp>
  {
    return l >> r;
  }

} // namespace details