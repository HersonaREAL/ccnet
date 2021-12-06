cmake_minimum_required(VERSION 3.4)

project(ccnet VERSION 1.0
              LANGUAGES CXX)

# c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIB_SRC
    ccnet/log/logAppender.cpp
    ccnet/log/logFormatter.cpp
    ccnet/log/logger.cpp
    ccnet/utils/utils.cpp
    ccnet/config/config.cpp
)

add_library(ccnet SHARED ${LIB_SRC})

add_executable(logtest tests/logtest.cpp)
add_dependencies(logtest ccnet)
target_link_libraries(logtest PUBLIC ccnet)


add_executable(configtest tests/configtest.cpp)
add_dependencies(configtest ccnet)
target_link_libraries(configtest PUBLIC ccnet)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)