#!/bin/bash

ARCHIVE_NAME="spm19-dimauro"
INCLUDE_FILES="include/barrier.hpp include/bsp.hpp include/lockableVector.hpp include/superstep.hpp include/uTimer.hpp include/workerThread.hpp"
SOURCE_FILES="src/barrier.cpp src/tiskin.cpp src/workerThread.cpp"
TESTS_FILES="tests/bspTester.cpp"

rm $ARCHIVE_NAME.zip

zip -T $ARCHIVE_NAME CMakeLists.txt $INCLUDE_FILES $SOURCE_FILES $TESTS_FILES