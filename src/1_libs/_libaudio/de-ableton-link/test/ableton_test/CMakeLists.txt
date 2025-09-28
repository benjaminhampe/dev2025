set(LIB_NAME de_ableton_link)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/ableton/

set(link_test_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ableton/test)
set(link_test_HEADERS
  ${link_discovery_DIR}/test/Interface.hpp
  ${link_discovery_DIR}/test/PayloadEntries.hpp
  ${link_discovery_DIR}/test/Socket.hpp
  ${link_util_DIR}/test/IoService.hpp
  ${link_util_DIR}/test/Timer.hpp
  ${link_test_DIR}/CatchWrapper.hpp
  ${link_test_DIR}/serial_io/Context.hpp
  ${link_test_DIR}/serial_io/Fixture.hpp
  ${link_test_DIR}/serial_io/SchedulerTree.hpp
  ${link_test_DIR}/serial_io/Timer.hpp
  PARENT_SCOPE
)


	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/bt_matchfinder.h

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
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _bespoke)
    else()
		if(OPENEXR_ENABLE_THREADING)
		# target_link_libraries(${LIB_NAME} PUBLIC Threads::Threads)
		endif()
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
