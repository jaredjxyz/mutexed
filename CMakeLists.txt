cmake_minimum_required(VERSION 3.16)
project(mutexed)

set(PACKAGE_VERSION 0.0.1)

option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(mutexed INTERFACE)
target_include_directories(mutexed INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)



install(TARGETS mutexed
        EXPORT mutexedTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    mutexedConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
    )
install(EXPORT mutexedTargets
        FILE mutexedConfig.cmake
        NAMESPACE mutexed::
        DESTINATION lib/cmake/mutexed
         )
export(TARGETS mutexed NAMESPACE mutexed:: FILE mutexedTargets.cmake)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include)

  if (BUILD_EXAMPLES)
    add_executable(hello_world examples/hello_world.cpp)
    target_link_libraries(hello_world mutexed)
  endif()
  if ("${BUILD_TESTS}")
    find_package(GTest REQUIRED)

    add_executable(mutexed_test test/test_mutexed.cpp)
    target_link_libraries(mutexed_test GTest::Main mutexed)
    enable_testing()

    gtest_discover_tests(mutexed_test)
endif()
