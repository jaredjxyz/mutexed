cmake_minimum_required(VERSION 3.16)
project(borrowing_test)

find_package(borrowing REQUIRED)
find_package(GTest REQUIRED)

add_executable(borrowing_test test_borrowing.cpp)
target_link_libraries(borrowing_test gtest_main borrowing::borrowing)