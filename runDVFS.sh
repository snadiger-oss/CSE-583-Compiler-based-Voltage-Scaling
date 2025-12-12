#!/bin/sh

rm -rf build
mkdir build
cd build
cmake ..
make -j
cd ../benchmarks
clang -O3 -S test_kernels.cpp -o test.s
clang -O3 -emit-llvm -S test_kernels.cpp -o test.ll
python3 ../scripts/auto_mca.py test.s --outdir mca
cd ../build

opt \
  -load-pass-plugin custom_passes/Slack/Slack.so \
  -load-pass-plugin custom_passes/DVFS/DVFS.so \
  -passes="slack-pass,DVFS,slack-energy" \
  ../benchmarks/test.ll

cd ../benchmarks

clang -c ../runtime/dvfs_runtime.c -o dvfs_runtime.o
opt -load-pass-plugin ../build/custom_passes/Slack/Slack.so \
    -load-pass-plugin ../build/custom_passes/DVFS/DVFS.so \
    -passes="slack-pass,DVFS,slack-energy" \
    -S test.ll -o instrumented.ll

clang -c instrumented.ll -o instrumented.o
clang++ instrumented.o dvfs_runtime.o -o dvfs_test
./dvfs_test

rm dvfs_test *.ll *.o *.s