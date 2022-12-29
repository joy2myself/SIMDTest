#!/bin/bash

bench_name=("AXPY" "RGB2YUV" "jacobi-2d" "nbody" "julia" "quadratic" "mandelbrot" "De-quantization" "black_scholes" "newton_fractal" "conv_maxpooling")
ISA_name=("sse2" "sse42" "avx" "avx2")

#check input

if [ -z "$1" ]; then
    echo "Error: No ISA specified"
    exit 1
fi
if ! [[ " ${ISA_name[@]} " =~ " ${1} " ]]; then
    echo "Error: Please pass one of 'sse2', 'sse42', 'avx', 'avx2'"
    exit 1
fi


if [ -z "$2" ]; then
    echo "Error: No bench name specified"
    exit 1
fi
if ! [[ " ${bench_name[@]} " =~ " ${2} " ]]; then
    echo "Error: Please pass one of 'AXPY', 'RGB2YUV', 'jacobi-2d', 'nbody', 'julia', 'quadratic', 'mandelbrot', 'De-quantization', 'black_scholes', 'newton_fractal', 'conv_maxpooling' "
    exit 1
fi

cd build/benchmark/src/test_codesize/

for files in *
do
  if [ -d "$files" ] && [ "$files" != "CMakeFiles" ] && [ "$files" == "$2" ] ; then
    cd "$files"
    echo "========================================="
    echo "             $files-$1"
    echo "========================================="
    cd bin
    for file in *
    do
        if [[ "$file" == *"_$1_"* ]] && [[ -x "$file" ]]; then
            simd=$(rev <<< "$file" | cut -d "_" -f 1 | rev)
            file_size=$(stat -c%s "$file")
            code_size=$(objdump -h "$file" | grep .text | awk '{print $3}')
            printf "$simd: %d bytes\n" $((0x$code_size))
        fi
    done
    cd ../../
  fi
done

