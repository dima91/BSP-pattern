#|/bin/bash

# Increasing performances
echo "Running testRun_1_1"
./tests/testRunner.sh 5 131072 1 1 2 1 > testsResults/testRun_1_1

echo "Running testRun_2_1"
./tests/testRunner.sh 5 131072 2 1 2 1 > testsResults/testRun_2_1

echo "Running testRun_2_2"
./tests/testRunner.sh 5 131072 2 2 2 1 > testsResults/testRun_2_2

echo "Running testRun_4_4"
./tests/testRunner.sh 5 131072 4 4 2 1 > testsResults/testRun_4_4

echo "Running testRun_8_4"
./tests/testRunner.sh 5 131072 8 4 2 1 > testsResults/testRun_8_4

echo "Running testRun_8_8"
./tests/testRunner.sh 5 131072 8 8 2 1 > testsResults/testRun_8_8

echo "Running testRun_16_8"
./tests/testRunner.sh 5 131072 16 8 2 1 > testsResults/testRun_16_8

echo "Running testRun_16_16"
./tests/testRunner.sh 5 131072 16 16 2 1 > testsResults/testRun_16_16

# Decreasing performances
echo "Running testRun_32_16"
./tests/testRunner.sh 5 131072 32 16 2 1 > testsResults/testRun_32_16

echo "Running testRun_32_32"
./tests/testRunner.sh 5 131072 32 32 2 1 > testsResults/testRun_32_32