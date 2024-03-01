include_directories(
  ${CGAL_INCLUDE_DIR}
)

set(programs "")

set(utility_sources Utility.cpp Context_selector.cpp)

add_executable(encode_mesh encode_mesh.cpp encoder.cpp ${utility_sources})
target_link_libraries(encode_mesh SPL ${CGAL_LIBRARY} ${GMP_LIBRARIES})
list(APPEND programs encode_mesh)

add_executable(decode_mesh decode_mesh.cpp decoder.cpp ${utility_sources})
target_link_libraries(decode_mesh SPL ${CGAL_LIBRARY} ${GMP_LIBRARIES})
list(APPEND programs decode_mesh)

install(TARGETS ${programs} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(EB_ENABLE_TEST AND BASH_PROGRAM)
	add_test(run_test_encode_decode
	  "${BASH_PROGRAM}" "${CMAKE_BINARY_DIR}/bin/wrapper"
	  "${CMAKE_SOURCE_DIR}/bin/run_test_encode_decode")
    set_tests_properties(run_test_encode_decode PROPERTIES TIMEOUT 60)
	add_custom_target(check
	  COMMAND
	  ${CMAKE_CTEST_COMMAND} -V
	  DEPENDS
	  ${programs} "${CMAKE_BINARY_DIR}/bin/wrapper"
	) 
else()
	message(WARNING "The test suite has been disabled.")
endif()
