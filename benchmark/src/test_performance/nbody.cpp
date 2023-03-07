#define ANKERL_NANOBENCH_IMPLEMENT
#include "../../all.h"
#include "nanobench/src/include/nanobench.h"

#include <cmath>
#include <cstdio>
#include <memory>
#include <numeric>
#include <random>

using ElemType = float;
constexpr auto kNruns = 1;
constexpr auto kN = 1024;
float posx[kN];
float posy[kN];
float posz[kN];
float velx[kN];
float vely[kN];
float velz[kN];
float mass[kN];

constexpr float eps2 = 0.01f;
constexpr float timeStep = 0.0001f;

struct NBODY_SCALAR
{
  inline void pPInteraction(
      float p1posx,
      float p1posy,
      float p1posz,
      float &p1velx,
      float &p1vely,
      float &p1velz,
      float p2posx,
      float p2posy,
      float p2posz,
      float p2mass)
  {
    const float xdistance = p1posx - p2posx;
    const float ydistance = p1posy - p2posy;
    const float zdistance = p1posz - p2posz;
    const float xdistanceSqr = xdistance * xdistance;
    const float ydistanceSqr = ydistance * ydistance;
    const float zdistanceSqr = zdistance * zdistance;
    const float distSqr = eps2 + xdistanceSqr + ydistanceSqr + zdistanceSqr;
    const float distSixth = distSqr * distSqr * distSqr;
    const float invDistCube = 1.0f / std::sqrt(distSixth);
    const float sts = p2mass * invDistCube * timeStep;
    p1velx += xdistanceSqr * sts;
    p1vely += ydistanceSqr * sts;
    p1velz += zdistanceSqr * sts;
  }
  void TestNBody(float *posx, float *posy, float *posz, float *velx, float *vely, float *velz, float *mass, size_t kN)
  {

    for (std::size_t i = 0; i < kN; i++)
    {
      const float piposx = posx[i];
      const float piposy = posy[i];
      const float piposz = posz[i];
      float pivelx = velx[i];
      float pively = vely[i];
      float pivelz = velz[i];
      for (std::size_t j = 0; j < kN; j++)
        pPInteraction(piposx, piposy, piposz, pivelx, pively, pivelz, posx[j], posy[j], posz[j], mass[j]);
      velx[i] = pivelx;
      vely[i] = pively;
      velz[i] = pivelz;
    }
    for (std::size_t i = 0; i < kN; i++)
    {
      posx[i] += velx[i] * timeStep;
      posy[i] += vely[i] * timeStep;
      posz[i] += velz[i] * timeStep;
    }
  }
  void operator()(float *posx, float *posy, float *posz, float *velx, float *vely, float *velz, float *mass, size_t kN)
  {
    TestNBody(posx, posy, posz, velx, vely, velz, mass, kN);
  }
};

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

template<typename F> void bench(ankerl::nanobench::Bench &bench, char const *name)
{
  F func;
  bench.minEpochIterations(100).run(name, [&]() { func(posx, posy, posz, velx, vely, velz, mass, kN); });
}
int main()
{
  std::random_device rng;
  std::default_random_engine engine{ rng() };
  std::normal_distribution<float> dist{ 0.0f, 1.0f };

  for (std::size_t i = 0; i < kN; ++i)
  {
    posx[i] = dist(engine);
    posy[i] = dist(engine);
    posz[i] = dist(engine);
    velx[i] = dist(engine) / 10.0f;
    vely[i] = dist(engine) / 10.0f;
    velz[i] = dist(engine) / 10.0f;
    mass[i] = dist(engine) / 100.0f;
  }

  ankerl::nanobench::Bench b_native;
  b_native.title("NBODY_TEST_NATIVE").unit("NBODY_NATIVE").relative(true);
  b_native.performanceCounters(true);
  bench<NBODY_SCALAR>(b_native, "scalar");
  bench<NBODY_SIMD<scalar_t_v<ElemType>, ElemType>>(b_native, "scalar_core");
  // bench<NBODY_SIMD<array_t_v<ElemType, LEN>, array_t_m<ElemType>, ElemType>>(b_native, "array");
  bench<NBODY_SIMD<xsimd_t_v_native<ElemType>, ElemType>>(b_native, "xsimd");
  bench<NBODY_SIMD<std_simd_t_v_native<ElemType>, ElemType>>(b_native, "std_simd");
  //bench<NBODY_SIMD<vcl_t_v_native<ElemType>, ElemType>>(b_native, "vcl");
  bench<NBODY_SIMD<highway_t_v_native<ElemType>, ElemType>>(b_native, "highway");
  // bench<NBODY_SIMD<nsimd_t_v_native<ElemType>, ElemType>>(b_native, "nsimd");
  bench<NBODY_SIMD<tsimd_t_v_native<ElemType>, ElemType>>(b_native, "tsimd");
  bench<NBODY_SIMD<eve_t_v_native<ElemType>, ElemType>>(b_native, "eve");
  bench<NBODY_SIMD<mipp_t_v_native<ElemType>, ElemType>>(b_native, "mipp");
  bench<NBODY_SIMD<vc_t_v_native<ElemType>, ElemType>>(b_native, "vc");

  return 0;
}