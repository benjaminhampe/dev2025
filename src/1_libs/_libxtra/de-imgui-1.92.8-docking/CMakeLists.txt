set(LIB_NAME de_imgui_1_92_8_docking)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/imgui.h
        ${LIB_HEADER_DIR}/imconfig.h
        ${LIB_HEADER_DIR}/imgui_internal.h
        ${LIB_HEADER_DIR}/imgui_impl_glfw.h
        ${LIB_HEADER_DIR}/imgui_impl_opengl3.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        ${LIB_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
        # ${LIB_SOURCE_DIR}/misc/cpp/README.txt

        ${LIB_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
        ${LIB_SOURCE_DIR}/misc/freetype/imgui_freetype.h
        # ${LIB_SOURCE_DIR}/misc/freetype/README.md
        ${LIB_SOURCE_DIR}/imgui.cpp
        ${LIB_SOURCE_DIR}/imgui_demo.cpp
        ${LIB_SOURCE_DIR}/imgui_draw.cpp
        ${LIB_SOURCE_DIR}/imgui_tables.cpp
        ${LIB_SOURCE_DIR}/imgui_widgets.cpp
        ${LIB_SOURCE_DIR}/imstb_rectpack.h
        ${LIB_SOURCE_DIR}/imstb_textedit.h
        ${LIB_SOURCE_DIR}/imstb_truetype.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_allegro5.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_allegro5.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_android.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_android.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx10.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx10.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx11.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx12.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_dx9.h
        ${LIB_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_glfw.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_glut.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_glut.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_metal.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_metal.mm
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_null.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_null.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_opengl2.h
        ${LIB_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        ${LIB_SOURCE_DIR}/backends/imgui_impl_opengl3_loader.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_osx.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_osx.mm
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdl2.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdl3.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_vulkan.h
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_wgpu.cpp
        # ${LIB_SOURCE_DIR}/backends/imgui_impl_wgpu.h
        ${LIB_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${LIB_SOURCE_DIR}/backends/imgui_impl_win32.h
        # ${LIB_SOURCE_DIR}/backends/sdlgpu3
        # ${LIB_SOURCE_DIR}/backends/sdlgpu3/build_instructions.txt
        # ${LIB_SOURCE_DIR}/backends/sdlgpu3/shader.frag
        # ${LIB_SOURCE_DIR}/backends/sdlgpu3/shader.vert
        # ${LIB_SOURCE_DIR}/backends/vulkan
        # ${LIB_SOURCE_DIR}/backends/vulkan/build_instructions.txt
        # ${LIB_SOURCE_DIR}/backends/vulkan/generate_spv.sh
        # ${LIB_SOURCE_DIR}/backends/vulkan/glsl_shader.frag
        # ${LIB_SOURCE_DIR}/backends/vulkan/glsl_shader.vert
    )

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_IMPLEMENTATION=1)
    target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS=1)
    target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_ENABLE_FREETYPE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_ENABLE_FREETYPE_LUNASVG=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE IMGL3W_IMPL=1)

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

    target_link_libraries(${LIB_NAME} PUBLIC de_glfw)
    target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)

endif()
