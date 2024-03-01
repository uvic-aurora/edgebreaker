cmake_minimum_required(VERSION 3.14)

set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)

project(edgebreaker LANGUAGES CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#include(FetchContent)
include(GNUInstallDirs)
include(Sanitizers)
include(CTest)

option(EB_ENABLE_TEST "Enable tests" TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_VERBOSE_MAKEFILE true)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" AND SPL_ROUNDING_MATH)
	add_definitions("-frounding-math")
	message("adding -frounding-math to compiler flags")
endif()

find_program(BASH_PROGRAM bash)
if(EB_ENABLE_TEST AND NOT BASH_PROGRAM)
	message(FATAL_ERROR "cannot run tests without bash")
endif()

set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
find_package(CGAL REQUIRED)
message("CGAL_INCLUDE_DIRS ${CGAL_INCLUDE_DIRS}")

configure_file("${CMAKE_SOURCE_DIR}/bin/wrapper.in"
  "${CMAKE_BINARY_DIR}/bin/wrapper" @ONLY)

set(SPL_ROOT_DIR ${CMAKE_SOURCE_DIR}/thirdparty/SPL-2.0.8)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} ${SPL_ROOT_DIR}/build/cmake/modules)
add_subdirectory(${SPL_ROOT_DIR})

message("SPL_INCLUDE_DIRS ${SPL_INCLUDE_DIRS}")
message("SPL_LIBRARIES ${SPL_LIBRARIES}")
message("CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}")

add_subdirectory(src)
