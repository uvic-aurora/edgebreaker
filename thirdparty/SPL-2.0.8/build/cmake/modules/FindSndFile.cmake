find_path(SNDFILE_INCLUDE_DIR NAMES sndfile.h)
set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})

find_library(SNDFILE_LIBRARY NAMES sndfile)
set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})

if(SNDFILE_INCLUDE_DIRS AND SNDFILE_LIBRARIES)
	set(SNDFILE_FOUND true)
endif()
if(SNDFILE_FOUND)
	if(NOT SndFile_FIND_QUIETLY)
		message(STATUS "Found libsndfile: ${SNDFILE_LIBRARIES}")
	endif()
else()
	if(SndFile_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find libsndfile")
	endif()
endif()
mark_as_advanced(SNDFILE_INCLUDE_DIRS SNDFILE_LIBRARIES)