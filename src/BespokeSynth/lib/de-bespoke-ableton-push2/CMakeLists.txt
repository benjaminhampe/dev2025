set(LIB_NAME de_bespoke_ableton_push2)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/push2/JuceToPush2DisplayBridge.h
        ${LIB_HEADER_DIR}/push2/Macros.h
        ${LIB_HEADER_DIR}/push2/Push2-Bitmap.h
        ${LIB_HEADER_DIR}/push2/Push2-Display.h
        ${LIB_HEADER_DIR}/push2/Push2-UsbCommunicator.h
        ${LIB_HEADER_DIR}/push2/Result.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/JuceToPush2DisplayBridge.cpp
        ${LIB_SOURCE_DIR}/Push2-Display.cpp
        ${LIB_SOURCE_DIR}/Push2-Usb-Communicator.cpp
        ${LIB_SOURCE_DIR}/Result.cpp
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")	
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/push2)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (WIN32)
        target_compile_definitions(${LIB_NAME} PRIVATE BESPOKE_PUSH2_SUPPORT=1)
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC de_bespoke_libusb)
	
    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _bespoke)
    else()
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()


    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
