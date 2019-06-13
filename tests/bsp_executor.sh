#|/bin/bash

ITERATIONS=5
ELEMENTS=4096


echo "Running testRun_1_1"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 1 1 2 1 > bspTestEvalResults/results_06.13//testRun_1_1

echo "Running testRun_2_1"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 2 1 2 1 > bspTestEvalResults/results_06.13//testRun_2_1

echo "Running testRun_2_2"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 2 2 2 1 > bspTestEvalResults/results_06.13//testRun_2_2

echo "Running testRun_4_4"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 4 4 2 1 > bspTestEvalResults/results_06.13//testRun_4_4

echo "Running testRun_8_4"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 8 4 2 1 > bspTestEvalResults/results_06.13//testRun_8_4

echo "Running testRun_8_8"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 8 8 2 1 > bspTestEvalResults/results_06.13//testRun_8_8

echo "Running testRun_12_8"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 12 8 2 1 > bspTestEvalResults/results_06.13//testRun_12_8

echo "Running testRun_16_8"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 16 8 2 1 > bspTestEvalResults/results_06.13//testRun_16_8

echo "Running testRun_16_10"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 16 10 2 1 > bspTestEvalResults/results_06.13//testRun_16_10

echo "Running testRun_16_12"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 16 12 2 1 > bspTestEvalResults/results_06.13//testRun_16_12

echo "Running testRun_16_14"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 16 14 2 1 > bspTestEvalResults/results_06.13//testRun_16_14

echo "Running testRun_16_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 16 16 2 1 > bspTestEvalResults/results_06.13//testRun_16_16

echo "Running testRun_18_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 18 16 2 1 > bspTestEvalResults/results_06.13//testRun_18_16

echo "Running testRun_20_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 20 16 2 1 > bspTestEvalResults/results_06.13//testRun_20_16

echo "Running testRun_22_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 22 16 2 1 > bspTestEvalResults/results_06.13//testRun_22_16

echo "Running testRun_24_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 24 16 2 1 > bspTestEvalResults/results_06.13//testRun_24_16

echo "Running testRun_26_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 26 16 2 1 > bspTestEvalResults/results_06.13//testRun_26_16

echo "Running testRun_28_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 28 16 2 1 > bspTestEvalResults/results_06.13//testRun_28_16

echo "Running testRun_30_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 20 16 2 1 > bspTestEvalResults/results_06.13//testRun_30_16

echo "Running testRun_32_16"
./tests/bsp_testRunner.sh $ITERATIONS $ELEMENTS 32 16 2 1 > bspTestEvalResults/results_06.13//testRun_32_16