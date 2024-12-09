#include "../../../../include/core/std_simd_core.h"
#include <nanobench.h>
using ElemType = float;

#include <cmath>
#include <cstdio>
#include <memory>
#include <numeric>
#include <random>

using ElemType = float;
constexpr auto kNruns = 1;
constexpr auto kN = 1024;

constexpr float eps2 = 0.01f;
constexpr float timeStep = 0.0001f;


template<typename Vec, typename Tp> struct NBODY_SIMD
{
  inline void pPInteractionSIMD(
      Vec p1posx,
      Vec p1posy,
      Vec p1posz,
      Vec &p1velx,
      Vec &p1vely,
      Vec &p1velz,
      Vec p2posx,
      Vec p2posy,
      Vec p2posz,
      Vec p2mass)
  {
    const Vec xdistance = p1posx - p2posx;
    const Vec ydistance = p1posy - p2posy;
    const Vec zdistance = p1posz - p2posz;
    const Vec xdistanceSqr = xdistance * xdistance;
    const Vec ydistanceSqr = ydistance * ydistance;
    const Vec zdistanceSqr = zdistance * zdistance;
    const Vec distSqr = details::BroadCast<Vec,Tp>(eps2) + xdistanceSqr + ydistanceSqr + zdistanceSqr;
    const Vec distSixth = distSqr * distSqr * distSqr;
    const Vec invDistCube = details::BroadCast<Vec,Tp>(1.0f) / details::Sqrt<Tp>(distSixth);
    const Vec sts = p2mass * invDistCube * details::BroadCast<Vec,Tp>(timeStep);
    p1velx += xdistanceSqr * sts;
    p1vely += ydistanceSqr * sts;
    p1velz += zdistanceSqr * sts;
  }
  void TestNBodySIMD(float *posx, float *posy, float *posz, float *velx, float *vely, float *velz, float *mass, size_t kN)
  {
    for (std::size_t i = 0; i < kN; i += details::Len<Vec, Tp>())
    {
      const Vec piposx = (Vec &)(posx[i]);
      const Vec piposy = (Vec &)(posy[i]);
      const Vec piposz = (Vec &)(posz[i]);
      Vec pivelx = (Vec &)(velx[i]);
      Vec pively = (Vec &)(vely[i]);
      Vec pivelz = (Vec &)(velz[i]);
      for (std::size_t j = 0; j < kN; j++)
      {
        Vec pjposx = details::BroadCast<Vec,Tp>(posx[j]);
        Vec pjposy = details::BroadCast<Vec,Tp>(posy[j]);
        Vec pjposz = details::BroadCast<Vec,Tp>(posz[j]);
        Vec pjmass = details::BroadCast<Vec,Tp>(mass[j]);
        pPInteractionSIMD(
            piposx, piposy, piposz, pivelx, pively, pivelz, pjposx, pjposy, pjposz, pjmass);
      }
      (Vec &)(velx[i]) = pivelx;
      (Vec &)(vely[i]) = pively;
      (Vec &)(velz[i]) = pivelz;
    }
    for (std::size_t i = 0; i < kN; i += details::Len<Vec, Tp>())
    {
      (Vec &)(posx[i]) += (const Vec &)(velx[i]) * details::BroadCast<Vec,Tp>(timeStep);
      (Vec &)(posy[i]) += (const Vec &)(vely[i]) * details::BroadCast<Vec,Tp>(timeStep);
      (Vec &)(posz[i]) += (const Vec &)(velz[i]) * details::BroadCast<Vec,Tp>(timeStep);
    }
  }
  void operator()(float *posx, float *posy, float *posz, float *velx, float *vely, float *velz, float *mass, size_t kN)
  {
    TestNBodySIMD(posx, posy, posz, velx, vely, velz, mass, kN);
  }
};

// 使用 nanobench 对simd实现进行性能测试
void test_std_simd(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN)
{
#if defined(USE_PLCT_SIMD)
  NBODY_SIMD<std_simd_t_v_native<ElemType>, ElemType> func;
  bench.minEpochIterations(100).run("plct_simd", [&]() {
    func(posx, posy, posz, velx, vely, velz, mass, kN);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#else
  NBODY_SIMD<std_simd_t_v_native<ElemType>, ElemType> func;
  // 配置 nanobench 的性能测试，指定最少迭代次数，执行simd实现并记录性能结果
  bench.minEpochIterations(100).run("std_simd", [&]() {
    func(posx, posy, posz, velx, vely, velz, mass, kN);
    ankerl::nanobench::doNotOptimizeAway(func);
  });
#endif
}