set(LIB_NAME de_rack2_jansson)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/jansson.h
        ${LIB_HEADER_DIR}/jansson_config.h
    )
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/dtoa.c
        ${LIB_SOURCE_DIR}/dump.c
        ${LIB_SOURCE_DIR}/error.c
        ${LIB_SOURCE_DIR}/hashtable.c
        ${LIB_SOURCE_DIR}/hashtable.h
        ${LIB_SOURCE_DIR}/hashtable_seed.c
        ${LIB_SOURCE_DIR}/jansson_private.h
        ${LIB_SOURCE_DIR}/jansson_private_config.h
        ${LIB_SOURCE_DIR}/load.c
        ${LIB_SOURCE_DIR}/lookup3.h
        ${LIB_SOURCE_DIR}/memory.c
        ${LIB_SOURCE_DIR}/pack_unpack.c
        ${LIB_SOURCE_DIR}/strbuffer.c
        ${LIB_SOURCE_DIR}/strbuffer.h
        ${LIB_SOURCE_DIR}/strconv.c
        ${LIB_SOURCE_DIR}/utf.c
        ${LIB_SOURCE_DIR}/utf.h
        ${LIB_SOURCE_DIR}/value.c
        ${LIB_SOURCE_DIR}/version.c
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
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
	
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBJANNSON=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
	
    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libsystem)
	else()
		#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
		#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
