#include "../../../include/core/nsimd_core.h"

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
  NBODY_SIMD<nsimd_t_v_native<ElemType>, ElemType>{}(posx, posy, posz, velx, vely, velz, mass, kN);
  return 0;
}
