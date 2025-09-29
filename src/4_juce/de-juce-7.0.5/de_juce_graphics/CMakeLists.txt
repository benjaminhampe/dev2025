set(LIB_NAME de_juce_graphics)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_graphics)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_graphics)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_graphics.h
      ${LIB_HEADER_DIR}/juce_graphics_config.h
      ${LIB_HEADER_DIR}/colour/juce_Colour.h
      ${LIB_HEADER_DIR}/colour/juce_ColourGradient.h
      ${LIB_HEADER_DIR}/colour/juce_Colours.h
      ${LIB_HEADER_DIR}/colour/juce_FillType.h
      ${LIB_HEADER_DIR}/colour/juce_PixelFormats.h
      ${LIB_HEADER_DIR}/contexts/juce_GraphicsContext.h
      ${LIB_HEADER_DIR}/contexts/juce_LowLevelGraphicsContext.h
      ${LIB_HEADER_DIR}/contexts/juce_LowLevelGraphicsPostScriptRenderer.h
      ${LIB_HEADER_DIR}/contexts/juce_LowLevelGraphicsSoftwareRenderer.h
      ${LIB_HEADER_DIR}/effects/juce_DropShadowEffect.h
      ${LIB_HEADER_DIR}/effects/juce_GlowEffect.h
      ${LIB_HEADER_DIR}/effects/juce_ImageEffectFilter.h
      ${LIB_HEADER_DIR}/fonts/juce_AttributedString.h
      ${LIB_HEADER_DIR}/fonts/juce_CustomTypeface.h
      ${LIB_HEADER_DIR}/fonts/juce_Font.h
      ${LIB_HEADER_DIR}/fonts/juce_GlyphArrangement.h
      ${LIB_HEADER_DIR}/fonts/juce_TextLayout.h
      ${LIB_HEADER_DIR}/fonts/juce_Typeface.h
      ${LIB_HEADER_DIR}/geometry/juce_AffineTransform.h
      ${LIB_HEADER_DIR}/geometry/juce_BorderSize.h
      ${LIB_HEADER_DIR}/geometry/juce_EdgeTable.h
      ${LIB_HEADER_DIR}/geometry/juce_Line.h
      ${LIB_HEADER_DIR}/geometry/juce_Parallelogram.h
      ${LIB_HEADER_DIR}/geometry/juce_Path.h
      ${LIB_HEADER_DIR}/geometry/juce_PathIterator.h
      ${LIB_HEADER_DIR}/geometry/juce_PathStrokeType.h
      ${LIB_HEADER_DIR}/geometry/juce_Point.h
      ${LIB_HEADER_DIR}/geometry/juce_Rectangle.h
      ${LIB_HEADER_DIR}/geometry/juce_RectangleList.h
      ${LIB_HEADER_DIR}/images/juce_Image.h
      ${LIB_HEADER_DIR}/images/juce_ImageCache.h
      ${LIB_HEADER_DIR}/images/juce_ImageConvolutionKernel.h
      ${LIB_HEADER_DIR}/images/juce_ImageFileFormat.h
      ${LIB_HEADER_DIR}/images/juce_ScaledImage.h
      #${LIB_HEADER_DIR}/native/juce_mac_CoreGraphicsContext.h
      #${LIB_HEADER_DIR}/native/juce_mac_CoreGraphicsContext.mm
      #${LIB_HEADER_DIR}/native/juce_mac_CoreGraphicsHelpers.h
      #${LIB_HEADER_DIR}/native/juce_mac_Fonts.mm
      ${LIB_HEADER_DIR}/native/juce_RenderingHelpers.h
      # ${LIB_HEADER_DIR}/native/juce_win32_Direct2DGraphicsContext.h
      ${LIB_HEADER_DIR}/placement/juce_Justification.h
      ${LIB_HEADER_DIR}/placement/juce_RectanglePlacement.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_graphics.cpp
      ${LIB_SOURCE_DIR}/colour/juce_Colour.cpp
      ${LIB_SOURCE_DIR}/colour/juce_ColourGradient.cpp
      ${LIB_SOURCE_DIR}/colour/juce_Colours.cpp
      ${LIB_SOURCE_DIR}/colour/juce_FillType.cpp
      ${LIB_SOURCE_DIR}/contexts/juce_GraphicsContext.cpp
      ${LIB_SOURCE_DIR}/contexts/juce_LowLevelGraphicsPostScriptRenderer.cpp
      ${LIB_SOURCE_DIR}/contexts/juce_LowLevelGraphicsSoftwareRenderer.cpp
      ${LIB_SOURCE_DIR}/effects/juce_DropShadowEffect.cpp
      ${LIB_SOURCE_DIR}/effects/juce_GlowEffect.cpp
      ${LIB_SOURCE_DIR}/fonts/juce_AttributedString.cpp
      ${LIB_SOURCE_DIR}/fonts/juce_CustomTypeface.cpp
      ${LIB_SOURCE_DIR}/fonts/juce_Font.cpp
      ${LIB_SOURCE_DIR}/fonts/juce_GlyphArrangement.cpp
      ${LIB_SOURCE_DIR}/fonts/juce_TextLayout.cpp
      ${LIB_SOURCE_DIR}/fonts/juce_Typeface.cpp
      ${LIB_SOURCE_DIR}/geometry/juce_AffineTransform.cpp
      ${LIB_SOURCE_DIR}/geometry/juce_EdgeTable.cpp
      ${LIB_SOURCE_DIR}/geometry/juce_Path.cpp
      ${LIB_SOURCE_DIR}/geometry/juce_PathIterator.cpp
      ${LIB_SOURCE_DIR}/geometry/juce_PathStrokeType.cpp
      ${LIB_SOURCE_DIR}/geometry/juce_Rectangle_test.cpp
      ${LIB_SOURCE_DIR}/images/juce_Image.cpp
      ${LIB_SOURCE_DIR}/images/juce_ImageCache.cpp
      ${LIB_SOURCE_DIR}/images/juce_ImageConvolutionKernel.cpp
      ${LIB_SOURCE_DIR}/images/juce_ImageFileFormat.cpp
      ${LIB_SOURCE_DIR}/image_formats/juce_GIFLoader.cpp
      ${LIB_SOURCE_DIR}/image_formats/juce_JPEGLoader.cpp
      ${LIB_SOURCE_DIR}/image_formats/juce_PNGLoader.cpp
      # ${LIB_SOURCE_DIR}/native/juce_android_Fonts.cpp
      # ${LIB_SOURCE_DIR}/native/juce_android_GraphicsContext.cpp
      # ${LIB_SOURCE_DIR}/native/juce_android_IconHelpers.cpp
      ${LIB_SOURCE_DIR}/native/juce_freetype_Fonts.cpp
      # ${LIB_SOURCE_DIR}/native/juce_linux_Fonts.cpp
      # ${LIB_SOURCE_DIR}/native/juce_linux_IconHelpers.cpp
      # ${LIB_SOURCE_DIR}/native/juce_mac_IconHelpers.cpp
      #${LIB_SOURCE_DIR}/native/juce_win32_Direct2DGraphicsContext.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_DirectWriteTypeface.cpp # included by juce_win32_Direct2DGraphicsContext.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_DirectWriteTypeLayout.cpp # included by juce_win32_Direct2DGraphicsContext.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_Fonts.cpp # included by juce_win32_Direct2DGraphicsContext.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_IconHelpers.cpp
      ${LIB_SOURCE_DIR}/placement/juce_RectanglePlacement.cpp
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_events)

   target_link_libraries(${LIB_NAME} PRIVATE de_png)
   target_link_libraries(${LIB_NAME} PRIVATE de_jpeg8d)
   #target_link_libraries(${LIB_NAME} PRIVATE de_tiff4)
   target_link_libraries(${LIB_NAME} PRIVATE de_gif5)
   target_link_libraries(${LIB_NAME} PRIVATE de_freetype2)

endif()

# add_subdirectory(test)

