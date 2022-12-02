#include "../../../include/core/eve_core.h"

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
  NBODY_SCALAR{}(posx, posy, posz, velx, vely, velz, mass, kN);
  return 0;
}
