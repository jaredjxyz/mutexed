cmake_minimum_required(VERSION 3.16)
project(borrowing)

set(PACKAGE_VERSION 0.0.1)

option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build TESTS" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(borrowing INTERFACE)
target_include_directories(borrowing INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)



install(TARGETS borrowing
        EXPORT borrowingTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    borrowingConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
    )
install(EXPORT borrowingTargets
        FILE borrowingConfig.cmake
        NAMESPACE borrowing::
        DESTINATION lib/cmake/borrowing
         )
export(TARGETS borrowing NAMESPACE borrowing:: FILE borrowingTargets.cmake)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include)

  if (BUILD_EXAMPLES)
    add_executable(hello_world examples/hello_world.cpp)
    target_link_libraries(hello_world borrowing)
  endif()
  if ("${BUILD_TESTS}")
  add_subdirectory(test)
endif()
