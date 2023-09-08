cmake_minimum_required(VERSION 3.22)
project(sylar)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "-ggdb -std=c++11")

set(LIB_SRC
    src/log.cpp
    src/util.cpp
    src/config.cpp
)

add_library(sylar SHARED ${LIB_SRC})

add_executable(logtest tests/logtest.cpp)
add_dependencies(logtest sylar)
target_link_libraries(logtest sylar yaml-cpp)

add_executable(configtest tests/configtest.cpp)
add_dependencies(configtest sylar)
target_link_libraries(configtest sylar yaml-cpp)


