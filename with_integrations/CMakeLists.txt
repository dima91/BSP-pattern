cmake_minimum_required (VERSION 3.5)
project (bsp_pattern)
cmake_policy (SET CMP0015 NEW)


option (PRINT_FULL "Print out more information about program's variables (e.g. input and output vectors)" OFF)
option (COMPUTE_SEQUENTIAL "Include computation of sequential sort of created vector" OFF)
option (COMPUTE_BARRIER_OVERHEAD "Include computation of the difference between time spent on computation and time spent on barriers waiting for others workers' end of computation" OFF)


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -pedantic -O2 -g -std=c++11 -pthread -fPIC")
set (EXECUTABLE_OUTPUT_PATH bin)


include_directories  (include)

## Creating .o files
set				(SOURCE_FILES src/barrier.cpp src/workerThread.cpp src/uTimer.cpp)
add_library		(${PROJECT_NAME}_objs OBJECT ${SOURCE_FILES})

## Creating static library
add_library		(${PROJECT_NAME} STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_objs>)


## Creating TiskinAlgorithm executable
add_executable				(TiskinAlgorithm src/tiskin.cpp)
target_link_libraries		(TiskinAlgorithm ${PROJECT_NAME})

# Creating BspTester executable
add_executable			(BspTester	tests/bspTester.cpp)
target_link_libraries	(BspTester	${PROJECT_NAME})


if (${COMPUTE_SEQUENTIAL})
	target_compile_definitions (TiskinAlgorithm PRIVATE COMPUTE_SEQUENTIAL)
endif ()


if (${PRINT_FULL})
	target_compile_definitions (TiskinAlgorithm PRIVATE PRINT_FULL)
endif ()

if (${COMPUTE_BARRIER_OVERHEAD})
	target_compile_definitions (TiskinAlgorithm PRIVATE COMPUTE_BARRIER_OVERHEAD)
	target_compile_definitions (BspTester PRIVATE COMPUTE_BARRIER_OVERHEAD)
endif ()