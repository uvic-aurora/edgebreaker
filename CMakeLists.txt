cmake_minimum_required(VERSION 3.11)

project(edgebreaker CXX C)

#include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_VERBOSE_MAKEFILE true)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" AND SPL_ROUNDING_MATH)
	add_definitions("-frounding-math")
	message("adding -frounding-math to compiler flags")
endif()

find_package(CGAL REQUIRED)
message("CGAL_INCLUDE_DIRS ${CGAL_INCLUDE_DIRS}")

cmake_policy(SET CMP0072 NEW)

#FetchContent_Declare(projB
#  GIT_REPOSITORY
#  "https://github.com/mdadams/SPL.git"
#  GIT_TAG
#  version-2.0.8
#)

# The SPL library was not found.
# Build the SPL library as an external project.
#FetchContent_GetProperties(projB)
#if(NOT projb_POPULATED)
#	FetchContent_Populate(projB)
#	set(CMAKE_MODULE_PATH
#	  ${CMAKE_MODULE_PATH} ${projb_SOURCE_DIR}/build/cmake/modules)
#	add_subdirectory(${projb_SOURCE_DIR} ${projb_BINARY_DIR})
#endif()

set(SPL_ROOT_DIR ${CMAKE_SOURCE_DIR}/thirdparty/SPL-2.0.8)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} ${SPL_ROOT_DIR}/build/cmake/modules)
add_subdirectory(${SPL_ROOT_DIR})

message("SPL_INCLUDE_DIRS ${SPL_INCLUDE_DIRS}")
message("SPL_LIBRARIES ${SPL_LIBRARIES}")
message("CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}")

add_subdirectory(src)
