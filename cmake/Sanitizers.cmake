option(ENABLE_ASAN "Enable Address Sanitizer" false)
option(ENABLE_USAN "Enable Undefined-Behavior Sanitizer" false)
option(ENABLE_LSAN "Enable Leak Sanitizer" false)
option(ENABLE_MSAN "Enable Memory Sanitizer" false)

macro(enable_sanitizer name)
	if ("${name}" MATCHES "asan")
		set(SANITIZER_COMPILE_FLAGS "-fsanitize=address")
		set(SANITIZER_LINKER_FLAGS "-fsanitize=address")
	elseif ("${name}" MATCHES "usan")
		set(SANITIZER_COMPILE_FLAGS "-fsanitize=undefined")
		set(SANITIZER_LINKER_FLAGS "-fsanitize=undefined")
	elseif ("${name}" MATCHES "leak")
		set(SANITIZER_COMPILE_FLAGS "-fsanitize=leak")
		set(SANITIZER_LINKER_FLAGS "-fsanitize=leak")
	elseif ("${name}" MATCHES "memory")
		set(SANITIZER_COMPILE_FLAGS "-fsanitize=memory")
		set(SANITIZER_LINKER_FLAGS "-fsanitize=memory")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_COMPILE_FLAGS}")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} ${SANITIZER_LINKER_FLAGS}")
endmacro()

message("ENABLE_ASAN ${ENABLE_ASAN}")
message("ENABLE_USAN ${ENABLE_USAN}")
message("ENABLE_LSAN ${ENABLE_LSAN}")
message("ENABLE_MSAN ${ENABLE_MSAN}")
if (ENABLE_ASAN)
	message("Enabling Address Sanitizer")
	enable_sanitizer("asan")
endif()
if (ENABLE_USAN)
	message("Enabling Undefined-Behavior Sanitizer")
	enable_sanitizer("usan")
endif()
if (ENABLE_LSAN)
	message("Enabling Leak Sanitizer")
	enable_sanitizer("lsan")
endif()
if (ENABLE_MSAN)
	message("Enabling Memory Sanitizer")
	enable_sanitizer("msan")
endif()
