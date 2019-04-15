configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/../include/SPL/config.hpp.in"
	"${CMAKE_CURRENT_BINARY_DIR}/../include/SPL/config.hpp"
)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/../include/SPL/version.hpp.in"
	"${CMAKE_CURRENT_BINARY_DIR}/../include/SPL/version.hpp"
)

set(spl_sources
	arithCoder.cpp
	audioFile.cpp
	bitStream.cpp
	filterDesign.cpp
	mCoder.cpp
	pnmCodec.cpp
	Sequence2.cpp
	Timer.cpp
)

set(spl_headers
	../include/SPL/Arcball.hpp
	../include/SPL/arithCoder.hpp
	../include/SPL/Array1.hpp
	../include/SPL/Array2.hpp
	../include/SPL/audioFile.hpp
	../include/SPL/bitStream.hpp
	../include/SPL/cgalUtil.hpp
	${CMAKE_CURRENT_BINARY_DIR}/../include/SPL/config.hpp
	../include/SPL/filterDesign.hpp
	../include/SPL/math.hpp
	../include/SPL/mCoder.hpp
	../include/SPL/misc.hpp
	../include/SPL/pnmCodec.hpp
	../include/SPL/Sequence1.hpp
	../include/SPL/Sequence2.hpp
	../include/SPL/Sequence.hpp
	../include/SPL/Timer.hpp
	${CMAKE_CURRENT_BINARY_DIR}/../include/SPL/version.hpp
)

add_library(SPL ${spl_sources} ${spl_headers})
target_include_directories(SPL PUBLIC 
	"${CMAKE_CURRENT_BINARY_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

install(TARGETS SPL DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES ${spl_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/SPL")
