cmake_minimum_required(VERSION 3.0)

project(edgebreaker CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_VERBOSE_MAKEFILE true)

find_package(SPL REQUIRED)
find_package(CGAL REQUIRED)

message("${SPL_INCLUDE_DIRS}")
message("${CGAL_INCLUDE_DIRS}")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" AND SPL_ROUNDING_MATH)
	add_definitions("-frounding-math")
	message("adding -frounding-math to compiler flags")
endif()

add_subdirectory(src)
