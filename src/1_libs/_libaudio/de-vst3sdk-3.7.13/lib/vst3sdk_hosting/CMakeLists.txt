set(LIB_NAME de_vst3sdk_hosting)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/connectionproxy.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/eventlist.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/hostclasses.h
    #   ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/hostdataexchangehandler.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/module.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/parameterchanges.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/pluginterfacesupport.h
    #   ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/plugprovider.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/processdata.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/optional.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/stringconvert.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/uid.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/versionparser.h
    )
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/connectionproxy.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/eventlist.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/hostclasses.cpp
    #   ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/hostdataexchangehandler.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/module.cpp
    #   ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/module_linux.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/module_win32.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/parameterchanges.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/pluginterfacesupport.cpp
    #   ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/plugprovider.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/processdata.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/stringconvert.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstinitiids.cpp
    )
	
	# if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_VST3SDK=1)

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting)

	# "$<$<CONFIG:Debug>:-DDEVELOPMENT=1>"
	# "$<$<CONFIG:Release>:-DRELEASE=1>"
	# "$<$<CONFIG:RelWithDebInfo>:-DRELEASE=1>"
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_definitions(${LIB_NAME} PRIVATE DEVELOPMENT=1)
	else()
		target_compile_definitions(${LIB_NAME} PRIVATE RELEASE=1)
	endif()
	
	target_link_libraries(${LIB_NAME} PUBLIC smtg_sdk_common)

endif()


