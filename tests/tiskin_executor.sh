#|/bin/bash

ITERATIONS=5

rm -fr tiskin_testsResults
mkdir tiskin_testsResults


# Increasing performances
echo "Running testRun_256_20"
ELEMENTS=$(echo '2^20' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_20

echo "Running testRun_256_22"
ELEMENTS=$(echo '2^22' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_22

echo "Running testRun_256_23"
ELEMENTS=$(echo '2^23' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_23

echo "Running testRun_256_24"
ELEMENTS=$(echo '2^24' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_24

echo "Running testRun_256_26"
ELEMENTS=$(echo '2^26' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_26

echo "Running testRun_256_27"
ELEMENTS=$(echo '2^27' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_27

echo "Running testRun_256_28"
ELEMENTS=$(echo '2^28' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_28

echo "Running testRun_256_29"
ELEMENTS=$(echo '2^29' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_29

echo "Running testRun_256_30"
ELEMENTS=$(echo '2^30' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_256_30







echo "Running testRun_64_18"
ELEMENTS=$(echo '2^18' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_64_18

echo "Running testRun_256_19"
ELEMENTS=$(echo '2^19' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_64_19

echo "Running testRun_256_20"
ELEMENTS=$(echo '2^20' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_64_20

echo "Running testRun_256_22"
ELEMENTS=$(echo '2^22' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_64_22

echo "Running testRun_64_24"
ELEMENTS=$(echo '2^24' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_64_24

echo "Running testRun_64_26"
ELEMENTS=$(echo '2^26' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_64_26




ELEMENTS=$(echo '2^28' | bc)

echo "Running testRun_p_32"
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 32 10 > tiskin_testsResults/testRun_p_32

echo "Running testRun_p_64"
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_p_64

echo "Running testRun_p_128"
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 128 10 > tiskin_testsResults/testRun_p_128

echo "Running testRun_p_256"
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_p_256

echo "Running testRun_p_512"
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 512 10 > tiskin_testsResults/testRun_p_512
