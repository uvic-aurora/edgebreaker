include_directories(
  ${SPL_INCLUDE_DIRS}
  ${CGAL_INCLUDE_DIR}
)

link_libraries(
  ${SPL_LIBRARIES}
  ${CGAL_LIBRARY}
  ${GMP_LIBRARIES}
)

set(utility_sources Utility.cpp Context_selector.cpp)
add_executable(encode_mesh encode_mesh.cpp encoder.cpp ${utility_sources})
add_executable(decode_mesh decode_mesh.cpp decoder.cpp ${utility_sources})
