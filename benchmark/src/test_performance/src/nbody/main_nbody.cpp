#define ANKERL_NANOBENCH_IMPLEMENT
#ifndef PLACE_HOLDER
#define PLACE_HOLDER 1
#endif
#include <cstdlib>
#include <nanobench.h>
#include <cmath>
#include <cstdio>
#include <memory>
#include <numeric>
#include <random>


using ElemType = float;
constexpr auto kNruns = 1;
constexpr auto kN = 1024;
alignas(64) float posx[kN];
alignas(64) float posy[kN];
alignas(64) float posz[kN];
alignas(64) float velx[kN];
alignas(64) float vely[kN];
alignas(64) float velz[kN];
alignas(64) float mass[kN];

constexpr float eps2 = 0.01f;
constexpr float timeStep = 0.0001f;

void test_scalar(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);

// #ifndef NSIMD_INEFFECTIVE
// void test_nsimd(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
// #endif

void test_std_simd(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
void test_vc(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
void test_tsimd(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
#if defined(__x86_64__) || defined(_M_X64)
  void test_vcl(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
#endif
void test_highway(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
void test_mipp(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
void test_eve(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);
void test_xsimd(ankerl::nanobench::Bench &bench, ElemType *posx, ElemType *posy, ElemType *posz, ElemType *velx, ElemType *vely, ElemType *velz, ElemType *mass, size_t kN);

void Initial()
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
}

int main()
{
    Initial();

    // 创建 nanobench 测试对象并配置基本参数，设置测试表头标题，启用性能计数器信息
    ankerl::nanobench::Bench b_native;
    b_native.title("nbody_TEST_NATIVE").unit("nbody_NATIVE").warmup(100).relative(true);
    b_native.performanceCounters(true);

    // 使用 nanobench 测试 标量程序 的性能
    test_scalar(b_native, posx, posy, posz, velx, vely, velz, mass, kN);

    // #ifndef NSIMD_INEFFECTIVE
    // test_nsimd(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
    // #endif
    
    // 使用 nanobench 测试 simd程序 的性能
    test_std_simd(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
    test_vc(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
    test_highway(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
    test_tsimd(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
    test_mipp(b_native, posx, posy, posz, velx, vely, velz, mass, kN);

    #if defined(__x86_64__) || defined(_M_X64)
      test_vcl(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
    #endif

    if (PLACE_HOLDER){
      test_xsimd(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
      test_eve(b_native, posx, posy, posz, velx, vely, velz, mass, kN);
    }
}