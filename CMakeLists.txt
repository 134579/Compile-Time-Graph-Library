cmake_minimum_required(VERSION 3.10)
project(ctgl)
add_executable(ctgl ./src/cpp/ctgl.cpp)
target_include_directories(ctgl PUBLIC src/h)

find_package(GTest REQUIRED)
enable_testing()
aux_source_directory(src/test SRC)

include(CTest)
add_executable(ctgl_test ${SRC})

add_test(NAME ctgl_test COMMAND ctgl_test)

target_link_libraries(ctgl_test PRIVATE GTest::gtest GTest::gtest_main)