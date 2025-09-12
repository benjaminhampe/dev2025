# It is only dark
# because i only live in the shadows of the irrlicht engine b(y|eim) Nikolaus
# and quake3 engine and halflife 2 source engine ( that has great physics ) and battlefield2 and such real realtime games.
# Most versatile and intelligent games are Gothic 1,2,3,?,Risen3 because replaying them is only repetetive for loot boxes and special weapons.
# Its hard todo deep games because its already hard todo gaming in RT and be beautiful and intelligent at once.
# e.g. The counterstrike source AI bots are so CPU heavy that you feel over 10 on most CPUs because it scales so bad.
# having over 20 bots is so bad that the rate drops below 10 fps completly unsync to screen rate or whatever. Just insane bad CPU cycle wasting.
# And this game had already like 27 iterations on professional teams since the predecessor counterstrike 1.6 over years and years ( no source ).
# Even multicore option ( probably only for GPU stuff ) does not help to scale better. Its hardcore CPU limiting by bad programming.
# Or the programmer had a better PC in 2008 than me in 2021 ( Zen2 2700 3.2 GHz 65W + R580X 200W + 32GB DDR4 RAM 40W + SanDisk 960 evo ~10W ).
# Maybe Radeon Driver issues, but i runs completly smoothly with 10 bot or so. Massive scaling impact on CPU side with this AI?

set(LIB_NAME DarkGPU)
project(${LIB_NAME}-23.04-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   # add_subdirectory(lib)

	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
	file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
	set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
	file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
	set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

	if (WIN32)
		enable_language(RC)
		list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
		list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
	endif()

	if (BUILD_SHARED)
		add_library(${LIB_NAME} SHARED "")
		message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
	else()
		add_library(${LIB_NAME} STATIC "")
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
	endif()

	target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/de)
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/de/gpu)
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/de/gpu/scene)
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/de/gpu/smesh)
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/de/gpu/smesh/io)

	if (WIN32)
        target_compile_definitions(${LIB_NAME} PRIVATE -DUNICODE=1)
        target_compile_definitions(${LIB_NAME} PRIVATE -D_UNICODE=1)
	endif()

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-variable)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-function)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-but-set-variable)

        if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
            target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
        endif()
    endif()
    
    target_link_libraries(${LIB_NAME} PUBLIC DarkImage)
    # target_link_libraries(${LIB_NAME} PUBLIC de_minizip)
    # target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)     # Encapsulated by SMeshIO, assimp is really big and mostly ugly, but i still love it since it knows what the beloved irrlicht-engine is and was.
    target_link_libraries(${LIB_NAME} PUBLIC de_glm)     # Encapsulated by SMeshIO, assimp is really big and mostly ugly, but i still love it since it knows what the beloved irrlicht-engine is and was.
    target_link_libraries(${LIB_NAME} PUBLIC de_opengles32)   # Public for all (hardcore) users, like me. Not liking me is your lack and problem, i guarantee.
    target_link_libraries(${LIB_NAME} PRIVATE de_assimp)     # Encapsulated by SMeshIO, assimp is really big and mostly ugly, but i still love it since it knows what the beloved irrlicht-engine is and was.
    target_link_libraries(${LIB_NAME} PRIVATE de_glfw)
    # target_link_libraries(${LIB_NAME} PRIVATE de_fftw3)    # Math
    # target_link_libraries(${LIB_NAME} PUBLIC de_boost)  # Math
    # target_link_libraries(${LIB_NAME} PUBLIC de_dvdcss)
    # target_link_libraries(${LIB_NAME} PUBLIC de_dvdnav)
    # target_link_libraries(${LIB_NAME} PUBLIC de_dvdread)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glfw)

endif()
# add_subdirectory(test)

