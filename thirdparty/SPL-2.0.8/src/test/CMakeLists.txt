link_libraries(
	SPL
	${CGAL_LIBRARY}
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	${Boost_LIBRARIES}
	${GMP_LIBRARIES}
	${SNDFILE_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

add_executable(array1_test array1_test.cpp)
add_executable(array2_test array2_test.cpp)
add_executable(convolve1 convolve1.cpp)
add_executable(convolve2 convolve2.cpp)
add_executable(pnm_test pnm_test.cpp)
add_executable(sequence1_test sequence1_test.cpp)
add_executable(sequence2_test sequence2_test.cpp)

set(test_programs
	run_array1_test
	run_array2_test
	run_sequence1_test
	run_sequence2_test
)
if (SH_PROGRAM)
	foreach (program ${test_programs})
		add_test(${program}
		  "${SH_PROGRAM}"
		  "${CMAKE_BINARY_DIR}/bin/test_wrapper"
		  "${CMAKE_CURRENT_SOURCE_DIR}/${program}"
		)
	endforeach()
endif()
