set(LIB_NAME de_ffmpeg)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADERS
        ${CMAKE_CURRENT_LIST_DIR}/include/de_libavcodec.hpp   
        ${CMAKE_CURRENT_LIST_DIR}/include/de/benniMP4.hpp)

    set(LIB_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/de/benniMP4.cpp)

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libavcodec.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/avcodec-58.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/avcodec-58.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libavdevice.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/avdevice-58.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/avdevice-58.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libavfilter.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/avfilter-7.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/avfilter-7.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libavformat.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/avformat-58.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/avformat-58.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libavutil.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/avutil-56.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/avutil-56.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libpostproc.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/postproc-55.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/postproc-55.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libswresample.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/swresample-3.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/swresample-3.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy libswscale.dll" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/swscale-5.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/swscale-5.dll)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy ffmpeg.exe" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/ffmpeg.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ffmpeg.exe)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy ffplay.exe" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/ffplay.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ffplay.exe)
    add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy ffprobe.exe" COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/bin/ffprobe.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ffprobe.exe)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
    target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

    target_link_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib)

    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libavcodec.dll.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libavdevice.dll.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libavfilter.dll.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libavformat.dll.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libavutil.dll.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libpostproc.dll.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libswresample.dll.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libswscale.dll.a)


endif()

add_subdirectory(test)