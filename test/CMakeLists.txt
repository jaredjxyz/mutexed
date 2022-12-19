cmake_minimum_required(VERSION 3.16)
project(borrowing_test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(borrowing REQUIRED)
find_package(GTest REQUIRED)

add_executable(borrowing_test test_borrowing.cpp)
target_link_libraries(borrowing_test GTest::Main borrowing::borrowing)

enable_testing()

gtest_discover_tests(borrowing_test)
