#!/bin/bash
#gcc -o dec2hex hex_dec.cc -D_DEC2HEX
#gcc -o hex2dec hex_dec.cc -D_HEX2DEC
gcc -o prime prime.c -D_PRIME -g -O0
gcc -o ssim measure.c -I../func_libs -D_MEASURE -DMEA_NAME=ssim -lm
gcc -o psnr measure.c -I../func_libs -D_MEASURE -DMEA_NAME=psnr -lm
