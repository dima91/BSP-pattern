#|/bin/bash

## 2^25
echo "Running TiskinTester 1"
./bin/TiskinTester 33554432 1 -s 100 > TiskinTester/06.16_2.25/results_1
sleep 5

echo "Running TiskinTester 2"
./bin/TiskinTester 33554432 2 -s 100 > TiskinTester/06.16_2.25/results_2
sleep 5

echo "Running TiskinTester 4"
./bin/TiskinTester 33554432 4 -s 100 > TiskinTester/06.16_2.25/results_4
sleep 5

echo "Running TiskinTester 8"
./bin/TiskinTester 33554432 8 -s 100 > TiskinTester/06.16_2.25/results_8
sleep 5

echo "Running TiskinTester 16"
./bin/TiskinTester 33554432 16 -s 100 > TiskinTester/06.16_2.25/results_16
sleep 5

echo "Running TiskinTester 32"
./bin/TiskinTester 33554432 32 -s 100 > TiskinTester/06.16_2.25/results_32
sleep 5

echo "Running TiskinTester 64"
./bin/TiskinTester 33554432 64 -s 100 > TiskinTester/06.16_2.25/results_64
sleep 5

echo "Running TiskinTester 128"
./bin/TiskinTester 33554432 128 -s 100 > TiskinTester/06.16_2.25/results_128
sleep 5

echo "Running TiskinTester 256"
./bin/TiskinTester 33554432 256 -s 100 > TiskinTester/06.16_2.25/results_256
sleep 5

echo "Running TiskinTester 512"
./bin/TiskinTester 33554432 512 -s 100 > TiskinTester/06.16_2.25/results_512
sleep 5




## 2^30

echo "Running TiskinTester 1"
./bin/TiskinTester 1073741824 1 -s 100 > TiskinTester/06.16_2.30/results_1
sleep 5

echo "Running TiskinTester 2"
./bin/TiskinTester 1073741824 2 -s 100 > TiskinTester/06.16_2.30/results_2
sleep 5

echo "Running TiskinTester 4"
./bin/TiskinTester 1073741824 4 -s 100 > TiskinTester/06.16_2.30/results_4
sleep 5

echo "Running TiskinTester 8"
./bin/TiskinTester 1073741824 8 -s 100 > TiskinTester/06.16_2.30/results_8
sleep 5

echo "Running TiskinTester 16"
./bin/TiskinTester 1073741824 16 -s 100 > TiskinTester/06.16_2.30/results_16
sleep 5

echo "Running TiskinTester 32"
./bin/TiskinTester 1073741824 32 -s 100 > TiskinTester/06.16_2.30/results_32
sleep 5

echo "Running TiskinTester 64"
./bin/TiskinTester 1073741824 64 -s 100 > TiskinTester/06.16_2.30/results_64
sleep 5

echo "Running TiskinTester 128"
./bin/TiskinTester 1073741824 128 -s 100 > TiskinTester/06.16_2.30/results_128
sleep 5

echo "Running TiskinTester 256"
./bin/TiskinTester 1073741824 256 -s 100 > TiskinTester/06.16_2.30/results_256
sleep 5

echo "Running TiskinTester 512"
./bin/TiskinTester 1073741824 512 -s 100 > TiskinTester/06.16_2.30/results_512