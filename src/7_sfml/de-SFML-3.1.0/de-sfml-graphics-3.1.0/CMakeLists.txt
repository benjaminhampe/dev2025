set(LIB_NAME de_sfml_graphics_310)
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

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SFML/GpuPreference.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics.hpp
        ${LIB_HEADER_DIR}/SFML/OpenGL.hpp

        ${LIB_HEADER_DIR}/SFML/Graphics/BlendMode.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/CircleShape.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Color.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Color.inl
        ${LIB_HEADER_DIR}/SFML/Graphics/ConvexShape.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/CoordinateType.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Drawable.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Export.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Font.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Glsl.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Glsl.inl
        ${LIB_HEADER_DIR}/SFML/Graphics/Glyph.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Image.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/PrimitiveType.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Rect.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Rect.inl
        ${LIB_HEADER_DIR}/SFML/Graphics/RectangleShape.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/RenderStates.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/RenderTarget.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/RenderTexture.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/RenderWindow.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Shader.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Shape.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Sprite.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/StencilMode.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Text.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Texture.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Transform.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Transform.inl
        ${LIB_HEADER_DIR}/SFML/Graphics/Transformable.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/Vertex.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/VertexArray.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/VertexBuffer.hpp
        ${LIB_HEADER_DIR}/SFML/Graphics/View.hpp
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/SFML/Graphics/BlendMode.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/CircleShape.cpp
        # ${LIB_SOURCE_DIR}/SFML/Graphics/CMakeLists.txt
        ${LIB_SOURCE_DIR}/SFML/Graphics/ConvexShape.cpp
        # ${LIB_SOURCE_DIR}/SFML/Graphics/Dependencies.cmake.in
        ${LIB_SOURCE_DIR}/SFML/Graphics/Font.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/GLCheck.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/GLCheck.hpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/GLExtensions.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/GLExtensions.hpp
        # ${LIB_SOURCE_DIR}/SFML/Graphics/GLExtensions.txt
        ${LIB_SOURCE_DIR}/SFML/Graphics/Glsl.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Image.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RectangleShape.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderStates.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderTarget.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderTexture.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderTextureImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderTextureImplDefault.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderTextureImplDefault.hpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderTextureImplFBO.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderTextureImplFBO.hpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/RenderWindow.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Shader.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Shape.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Sprite.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/StencilMode.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Text.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Texture.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/TextureSaver.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/TextureSaver.hpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Transform.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/Transformable.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/VertexArray.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/VertexBuffer.cpp
        ${LIB_SOURCE_DIR}/SFML/Graphics/View.cpp
    )

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC SFML_STATIC=1)
    endif()

    target_compile_definitions(${LIB_NAME} PRIVATE SFML_GRAPHICS_EXPORTS=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/extlibs/headers)

    # target_compile_definitions(${LIB_NAME} PRIVATE LUNASVG_BUILD)
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



    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_system_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_window_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    target_link_libraries(${LIB_NAME} PUBLIC de_SheenBidi_300)
    target_link_libraries(${LIB_NAME} PUBLIC de_harfbuzz_14_3_1)

endif()

add_subdirectory(test)
