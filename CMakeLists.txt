cmake_minimum_required (VERSION 3.5)
project (bsp_pattern)
cmake_policy (SET CMP0015 NEW)


option (BUILD_TESTS "Compile project unit tests" OFF)
option (PRINT_FULL "Print out more information about program's variables (e.g. input and output vectors)" OFF)
option (COMPUTE_SEQUENTIAL "Include computation of sequential sort of created vector" ON)


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -pedantic -O2 -g -std=c++11 -pthread -fPIC")
set (EXECUTABLE_OUTPUT_PATH bin)


include_directories  (include)

## Creating .o files
set				(SOURCE_FILES src/barrier.cpp src/workerThread.cpp)
add_library		(${PROJECT_NAME}_objs OBJECT ${SOURCE_FILES})

## Creating static library
add_library(${PROJECT_NAME} STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_objs>)
#set_target_properties (${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})


# Creating execuables
add_executable			(TiskinAlgorithm src/tiskin.cpp)
target_link_libraries	(TiskinAlgorithm ${PROJECT_NAME})

add_executable			(TiskinTrial src/tiskinTrial.cpp)
target_link_libraries	(TiskinTrial ${PROJECT_NAME})

## Creating shared library
#add_library(${PROJECT_NAME} SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_objs>)
#set_target_properties (${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})


## Test directories
## (To build tests directory, just defien BUILD_TESTS to ON (cmake -D BUILD_TESTS=ON .) when executing 'cmake' command)
if (${BUILD_TESTS})
    #add_subdirectory (tests)
endif ()