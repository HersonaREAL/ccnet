cmake_minimum_required(VERSION 3.4)

include(cmake/utils.cmake)

project(ccnet VERSION 1.0
              LANGUAGES CXX)

# c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_BUILD_TYPE "Debug") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -Wno-builtin-macro-redefined")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -Wno-builtin-macro-redefined")


#binary output
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
#include header
include_directories(ccnet/include)

#3rdparty
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
set(3rdparty
    yaml-cpp
)

#ccnet
file(GLOB_RECURSE LIB_SRC ccnet/*.cpp)
add_library(ccnet SHARED ${LIB_SRC})
# add_library(ccnet  ${LIB_SRC})
target_link_libraries(ccnet PUBLIC ${3rdparty})
force_redefine_file_macro_for_sources(ccnet)


#############################test#####################################
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
foreach(test_file ${TEST_FILES})
    STRING(REGEX MATCH "[^/]+$" testsourcefilewithoutpath ${test_file})    
    STRING(REPLACE ".cpp" "" testname ${testsourcefilewithoutpath})             
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/tests)                   
    ccnet_add_executable(${testname} ${test_file} ccnet ${3rdparty})
endforeach(test_file ${TEST_FILES})
#############################test#####################################


