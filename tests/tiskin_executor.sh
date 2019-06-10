#|/bin/bash

ITERATIONS=5

rm -fr tiskin_testsResults
mkdir tiskin_testsResults


# Increasing performances
echo "Running testRun_2_20"
ELEMENTS=$(echo '2^20' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_2_20

echo "Running testRun_2_22"
ELEMENTS=$(echo '2^22' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_2_22

echo "Running testRun_2_24"
ELEMENTS=$(echo '2^24' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_2_24

echo "Running testRun_2_26"
ELEMENTS=$(echo '2^26' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_2_26

echo "Running testRun_2_28"
ELEMENTS=$(echo '2^28' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_2_28

echo "Running testRun_2_29"
ELEMENTS=$(echo '2^29' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_2_29

echo "Running testRun_2_30"
ELEMENTS=$(echo '2^30' | bc)
./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_2_30




echo "Running testRun_"
ELEMENTS=$(echo '2^28' | bc)

./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 32 10 > tiskin_testsResults/testRun_p_64

./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 64 10 > tiskin_testsResults/testRun_p_64

./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 128 10 > tiskin_testsResults/testRun_p_128

./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 256 10 > tiskin_testsResults/testRun_p_256

./tests/tiskin_testRunner.sh $ITERATIONS $ELEMENTS 512 10 > tiskin_testsResults/testRun_p_512
