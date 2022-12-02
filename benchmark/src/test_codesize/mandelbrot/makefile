CXX = g++
SRC = ${wildcard *.cpp}
SHELL=/bin/bash

NSIMD_LIB_AVX = ../../../../simd_libraries/nsimd/build-avx-gcc/
NSIMD_LIB_AVX2 = ../../../../simd_libraries/nsimd/build-avx2-gcc/
NSIMD_LIB_SSE2 = ../../../../simd_libraries/nsimd/build-sse2-gcc/
NSIMD_LIB_SSE42 = ../../../../simd_libraries/nsimd/build-sse42-gcc/
NSIMD_NAME = nsimd_mandelbrot.cpp

CXXFLAGS = -O2 -std=c++2a

.PHONY : all
all: ALL_SSE2 ALL_SSE4_2 ALL_AVX ALL_AVX2 ALL_AVX2_FMA

ALL_SSE2:
	@for src in $(SRC) ; do \
		if [ $$src = $(NSIMD_NAME) ]; then \
		$(CXX) $(CXXFLAGS) -DSSE2 -msse2 -L $(NSIMD_LIB_SSE2) -lnsimd_sse2 $$src -o ./bin/sse2/$${src//.cpp/_sse2}; \
		else \
		$(CXX) $(CXXFLAGS) -msse2 $$src -o ./bin/sse2/$${src/.cpp/_sse2} ; \
		fi \
	done

ALL_SSE4_2:
	@for src in $(SRC) ; do \
		if [ $$src = $(NSIMD_NAME) ]; then \
		-$(CXX) $(CXXFLAGS) -DSSE42 -msse4.2 -L $(NSIMD_LIB_SSE42) -lnsimd_sse42 $$src -o ./bin/sse4.2/$${src//.cpp/_sse42}; \
		else \
		$(CXX) $(CXXFLAGS) -msse4.2 $$src -o ./bin/sse4.2/$${src/.cpp/_sse42} ; \
		fi \
	done

ALL_AVX:
	@for src in $(SRC) ; do \
		if [ $$src = $(NSIMD_NAME) ]; then \
		$(CXX) $(CXXFLAGS) -DAVX -mavx -L $(NSIMD_LIB_AVX) -lnsimd_avx $$src -o ./bin/avx/$${src//.cpp/_avx}; \
		else \
		$(CXX) $(CXXFLAGS) -mavx $$src -o ./bin/avx/$${src/.cpp/_avx} ; \
		fi \
	done

ALL_AVX2:
	@for src in $(SRC) ; do \
		if [ $$src = $(NSIMD_NAME) ]; then \
		$(CXX) $(CXXFLAGS) -DAVX2 -mavx2 -L $(NSIMD_LIB_AVX2) -lnsimd_avx2 $$src -o ./bin/avx2/$${src//.cpp/_avx2}; \
		else \
		$(CXX) $(CXXFLAGS) -mavx2 $$src -o ./bin/avx2/$${src/.cpp/_avx2} ; \
		fi \
	done

ALL_AVX2_FMA:
	@for src in $(SRC) ; do \
		if [ $$src = $(NSIMD_NAME) ]; then \
		$(CXX) $(CXXFLAGS) -DAVX2 -DFMA -mavx2 -mfma -L $(NSIMD_LIB_AVX2) -lnsimd_avx2 $$src -o ./bin/avx2_fma/$${src//.cpp/_avx2_fma}; \
		else \
		$(CXX) $(CXXFLAGS) -mavx2 -mfma $$src -o ./bin/avx2_fma/$${src/.cpp/_avx2_fma} ; \
		fi \
	done


.PHONY : clean
clean: clean_sse2 clean_sse4_2 clean_avx clean_avx2 clean_avx2_fma

clean_sse2:
	@rm -f ./bin/sse2/*
clean_sse4_2:
	@rm -f ./bin/sse4.2/*
clean_avx:
	@rm -f ./bin/avx/*
clean_avx2:
	@rm -f ./bin/avx2/*
clean_avx2_fma:
	@rm -f ./bin/avx2_fma/*