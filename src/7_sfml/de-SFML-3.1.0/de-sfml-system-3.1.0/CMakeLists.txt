set(LIB_NAME de_sfml_system_310)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    # set(LUNASVG_VERSION_MAJOR 3)
    # set(LUNASVG_VERSION_MINOR 1)
    # set(LUNASVG_VERSION_MICRO 0)

    set(SFML_OS_WINDOWS 1)
    # set(SFML_OS_LINUX 1)
    # set(SFML_OS_ANDROID 1)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SFML/Config.hpp
        ${LIB_HEADER_DIR}/SFML/System.hpp
        ${LIB_HEADER_DIR}/SFML/System/Angle.hpp
        ${LIB_HEADER_DIR}/SFML/System/Angle.inl
        ${LIB_HEADER_DIR}/SFML/System/Clock.hpp
        ${LIB_HEADER_DIR}/SFML/System/Err.hpp
        ${LIB_HEADER_DIR}/SFML/System/Exception.hpp
        ${LIB_HEADER_DIR}/SFML/System/Export.hpp
        ${LIB_HEADER_DIR}/SFML/System/FileInputStream.hpp
        ${LIB_HEADER_DIR}/SFML/System/InputStream.hpp
        ${LIB_HEADER_DIR}/SFML/System/MemoryInputStream.hpp
        ${LIB_HEADER_DIR}/SFML/System/NativeActivity.hpp
        ${LIB_HEADER_DIR}/SFML/System/Sleep.hpp
        ${LIB_HEADER_DIR}/SFML/System/String.hpp
        ${LIB_HEADER_DIR}/SFML/System/String.inl
        ${LIB_HEADER_DIR}/SFML/System/SuspendAwareClock.hpp
        ${LIB_HEADER_DIR}/SFML/System/Time.hpp
        ${LIB_HEADER_DIR}/SFML/System/Time.inl
        ${LIB_HEADER_DIR}/SFML/System/TimeoutWithPredicate.hpp
        ${LIB_HEADER_DIR}/SFML/System/Utf.hpp
        ${LIB_HEADER_DIR}/SFML/System/Utf.inl
        ${LIB_HEADER_DIR}/SFML/System/Vector2.hpp
        ${LIB_HEADER_DIR}/SFML/System/Vector2.inl
        ${LIB_HEADER_DIR}/SFML/System/Vector3.hpp
        ${LIB_HEADER_DIR}/SFML/System/Vector3.inl
        ${LIB_HEADER_DIR}/SFML/System/Version.hpp
        ${LIB_HEADER_DIR}/SFML/System/EnumArray.hpp
        ${LIB_HEADER_DIR}/SFML/System/Win32/SleepImpl.hpp
        ${LIB_HEADER_DIR}/SFML/System/Win32/WindowsHeader.hpp
        ${LIB_HEADER_DIR}/SFML/System/Utils.hpp
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/SFML/System/Android/Activity.cpp
        # ${LIB_SOURCE_DIR}/SFML/System/Android/Activity.hpp
        # ${LIB_SOURCE_DIR}/SFML/System/Android/NativeActivity.cpp
        # ${LIB_SOURCE_DIR}/SFML/System/Android/ResourceStream.cpp
        # ${LIB_SOURCE_DIR}/SFML/System/Android/ResourceStream.hpp
        # ${LIB_SOURCE_DIR}/SFML/System/Android/SuspendAwareClock.cpp
        ${LIB_SOURCE_DIR}/SFML/System/Clock.cpp
        # ${LIB_SOURCE_DIR}/SFML/System/CMakeLists.txt
        # ${LIB_SOURCE_DIR}/SFML/System/Dependencies.cmake.in
        ${LIB_SOURCE_DIR}/SFML/System/Err.cpp
        ${LIB_SOURCE_DIR}/SFML/System/FileInputStream.cpp
        ${LIB_SOURCE_DIR}/SFML/System/MemoryInputStream.cpp
        ${LIB_SOURCE_DIR}/SFML/System/Sleep.cpp
        ${LIB_SOURCE_DIR}/SFML/System/String.cpp
        ${LIB_SOURCE_DIR}/SFML/System/TimeoutWithPredicate.cpp

        # ${LIB_SOURCE_DIR}/SFML/System/Unix/SleepImpl.cpp
        # ${LIB_SOURCE_DIR}/SFML/System/Unix/SleepImpl.hpp

        ${LIB_SOURCE_DIR}/SFML/System/Utils.cpp

        ${LIB_SOURCE_DIR}/SFML/System/Version.cpp

        ${LIB_SOURCE_DIR}/SFML/System/Win32/SleepImpl.cpp
    )

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC SFML_STATIC=1)
    endif()

    target_compile_definitions(${LIB_NAME} PRIVATE SFML_SYSTEM_EXPORTS=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # set(LIB_EXTLIB_DIR ${LIB_SOURCE_DIR}/extlibs/headers)
    # target_include_directories(${LIB_NAME} SYSTEM PRIVATE "${LIB_EXTLIB_DIR}/cpp-unicodelib")

    target_compile_definitions(${LIB_NAME} PUBLIC SFML_IS_BIG_ENDIAN=0)
    target_compile_definitions(${LIB_NAME} PUBLIC SFML_OS_WINDOWS=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC SFML_OS_ANDROID=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC SFML_OS_LINUX=1)

    if(SFML_OS_LINUX)
        target_link_libraries(${LIB_NAME} PRIVATE rt)
    elseif(SFML_OS_WINDOWS)
        target_link_libraries(${LIB_NAME} PRIVATE winmm)
    elseif(SFML_OS_ANDROID)
        target_link_libraries(${LIB_NAME} PRIVATE android log)
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_extlibs)


    # target_compile_definitions(${LIB_NAME} PRIVATE LUNASVG_DISABLE_LOAD_SYSTEM_FONTS)

    #target_compile_definitions(${LIB_NAME} PUBLIC -D_IRR_STATIC_LIB_=1)
    # if (MSVC)
    # else()
    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    # endif()
    # target_link_libraries(${LIB_NAME} PUBLIC de_irrlicht)
    # target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    # #target_link_libraries(${LIB_NAME} PUBLIC de_glad)

    # target_link_libraries(${LIB_NAME} PUBLIC de_plutovg_131)

endif()

# add_subdirectory(test)
