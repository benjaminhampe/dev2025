set(LIB_NAME de_lua)
project(${LIB_NAME} C) # VERSION 5.4.7

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS 
		${LIB_HEADER_DIR}/lapi.h
		${LIB_HEADER_DIR}/lauxlib.h
		${LIB_HEADER_DIR}/lcode.h
		${LIB_HEADER_DIR}/lctype.h
		${LIB_HEADER_DIR}/ldebug.h
		${LIB_HEADER_DIR}/ldo.h
		${LIB_HEADER_DIR}/lfunc.h
		${LIB_HEADER_DIR}/lgc.h
		${LIB_HEADER_DIR}/ljumptab.h
		${LIB_HEADER_DIR}/llex.h
		${LIB_HEADER_DIR}/llimits.h
		${LIB_HEADER_DIR}/lmem.h
		${LIB_HEADER_DIR}/lobject.h
		${LIB_HEADER_DIR}/lopcodes.h
		${LIB_HEADER_DIR}/lopnames.h
		${LIB_HEADER_DIR}/lparser.h
		${LIB_HEADER_DIR}/lprefix.h
		${LIB_HEADER_DIR}/lstate.h
		${LIB_HEADER_DIR}/lstring.h
		${LIB_HEADER_DIR}/ltable.h
		${LIB_HEADER_DIR}/ltm.h
		${LIB_HEADER_DIR}/lua.h
		${LIB_HEADER_DIR}/lua.hpp
		${LIB_HEADER_DIR}/luaconf.h
		${LIB_HEADER_DIR}/lualib.h
		${LIB_HEADER_DIR}/lundump.h
		${LIB_HEADER_DIR}/lvm.h
		${LIB_HEADER_DIR}/lzio.h
    )
	
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/lapi.c
		${LIB_SOURCE_DIR}/lauxlib.c
		${LIB_SOURCE_DIR}/lbaselib.c
		${LIB_SOURCE_DIR}/lcode.c
		${LIB_SOURCE_DIR}/lcorolib.c
		${LIB_SOURCE_DIR}/lctype.c
		${LIB_SOURCE_DIR}/ldblib.c
		${LIB_SOURCE_DIR}/ldebug.c
		${LIB_SOURCE_DIR}/ldo.c
		${LIB_SOURCE_DIR}/ldump.c
		${LIB_SOURCE_DIR}/lfunc.c
		${LIB_SOURCE_DIR}/lgc.c
		${LIB_SOURCE_DIR}/linit.c
		${LIB_SOURCE_DIR}/liolib.c
		${LIB_SOURCE_DIR}/llex.c
		${LIB_SOURCE_DIR}/lmathlib.c
		${LIB_SOURCE_DIR}/lmem.c
		${LIB_SOURCE_DIR}/loadlib.c
		${LIB_SOURCE_DIR}/lobject.c
		${LIB_SOURCE_DIR}/lopcodes.c
		${LIB_SOURCE_DIR}/loslib.c
		${LIB_SOURCE_DIR}/lparser.c
		${LIB_SOURCE_DIR}/lstate.c
		${LIB_SOURCE_DIR}/lstring.c
		${LIB_SOURCE_DIR}/lstrlib.c
		${LIB_SOURCE_DIR}/ltable.c
		${LIB_SOURCE_DIR}/ltablib.c
		${LIB_SOURCE_DIR}/ltm.c
		${LIB_SOURCE_DIR}/lua.c
		${LIB_SOURCE_DIR}/lundump.c
		${LIB_SOURCE_DIR}/lutf8lib.c
		${LIB_SOURCE_DIR}/lvm.c
		${LIB_SOURCE_DIR}/lzio.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libextra)
	else()
	    # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()
    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
