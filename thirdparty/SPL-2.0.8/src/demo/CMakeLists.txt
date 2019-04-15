link_libraries(
	SPL
	${CGAL_LIBRARY}
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	${Boost_LIBRARIES}
	${SNDFILE_LIBRARIES}
	${GMP_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

add_executable(arithDecoder arithDecoder.cpp)
list(APPEND programs arithDecoder)

add_executable(arithEncoder arithEncoder.cpp)
list(APPEND programs arithEncoder)

add_executable(array1cmp array1cmp.cpp)
list(APPEND programs array1cmp)

add_executable(array2cmp array2cmp.cpp)
list(APPEND programs array2cmp)

add_executable(audioToText audioToText.cpp)
list(APPEND programs audioToText)

add_executable(cgalDemo cgalDemo.cpp)
list(APPEND programs cgalDemo)

add_executable(filterDesignDemo filterDesignDemo.cpp)
list(APPEND programs filterDesignDemo)

add_executable(imageFilter imageFilter.cpp)
list(APPEND programs imageFilter)

add_executable(imageDerivative imageDerivative.cpp)
list(APPEND programs imageDerivative)

add_executable(makeAudio makeAudio.cpp)
list(APPEND programs makeAudio)

add_executable(mandelbrot mandelbrot.cpp)
list(APPEND programs mandelbrot)

add_executable(sequence1 sequence1.cpp)
list(APPEND programs sequence1)

add_executable(sequence2 sequence2.cpp)
list(APPEND programs sequence2)

add_executable(timerDemo timerDemo.cpp)
list(APPEND programs timerDemo)

add_executable(wireframe wireframe.cpp)
list(APPEND programs wireframe)

install(TARGETS ${programs} DESTINATION ${CMAKE_INSTALL_BINDIR})
