set(LIB_NAME de_vst3sdk)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/public.sdk/source/common/pluginview.h
        ${LIB_HEADER_DIR}/public.sdk/source/main/moduleinit.h
        ${LIB_HEADER_DIR}/public.sdk/source/main/pluginfactory.h
        ${LIB_HEADER_DIR}/public.sdk/source/main/pluginfactory_constexpr.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/alignedalloc.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/audiobuffers.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/dataexchange.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/memoryibstream.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/processcontextrequirements.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/processdataslicer.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/ringbuffer.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/rttransfer.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/sampleaccurate.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/stringconvert.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/systemtime.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/testing.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/vst2persistence.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstaudioeffect.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstbus.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstbypassprocessor.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstcomponent.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstcomponentbase.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vsteditcontroller.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vsteventshelper.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vsthelpers.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstnoteexpressiontypes.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstparameters.h
		${LIB_HEADER_DIR}/public.sdk/source/vst/vstpresetfile.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstrepresentation.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstspeakerarray.h
    )	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/public.sdk/source/common/pluginview.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/main/moduleinit.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/main/pluginfactory.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/dataexchange.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/stringconvert.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/vst2persistence.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/systemtime.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/testing.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstaudioeffect.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstbus.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstcomponent.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstcomponentbase.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vsteditcontroller.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstinitiids.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstnoteexpressiontypes.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstparameters.cpp
		${LIB_SOURCE_DIR}/public.sdk/source/vst/vstpresetfile.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstrepresentation.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/common)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/main)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/vst)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/vst/utility)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

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


