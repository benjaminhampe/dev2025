set(LIB_NAME de_tuning_library) # cmake target name
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
	set(LIB_HEADERS
		"${LIB_HEADER_DIR}/Tunings.h"
		"${LIB_HEADER_DIR}/TuningsImpl.h"
	)

    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

	add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

	target_include_directories(${LIB_NAME} INTERFACE ${LIB_HEADER_DIR})

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBTUNING=1)

endif()
