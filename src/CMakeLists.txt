include_directories(
  ${CGAL_INCLUDE_DIR}
)

set(utility_sources Utility.cpp Context_selector.cpp)

add_executable(encode_mesh encode_mesh.cpp encoder.cpp ${utility_sources})
target_link_libraries(encode_mesh SPL ${CGAL_LIBRARY} ${GMP_LIBRARIES})

add_executable(decode_mesh decode_mesh.cpp decoder.cpp ${utility_sources})
target_link_libraries(decode_mesh SPL ${CGAL_LIBRARY} ${GMP_LIBRARIES})

install(TARGETS encode_mesh decode_mesh DESTINATION ${CMAKE_INSTALL_BINDIR})
