set(LIB_NAME de_juce_gui_extra)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_gui_extra)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_gui_extra)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_gui_extra.h
      ${LIB_HEADER_DIR}/juce_gui_extra_config.h
      ${LIB_HEADER_DIR}/code_editor/juce_CodeDocument.h
      ${LIB_HEADER_DIR}/code_editor/juce_CodeEditorComponent.h
      ${LIB_HEADER_DIR}/code_editor/juce_CodeTokeniser.h
      ${LIB_HEADER_DIR}/code_editor/juce_CPlusPlusCodeTokeniser.h
      ${LIB_HEADER_DIR}/code_editor/juce_CPlusPlusCodeTokeniserFunctions.h
      ${LIB_HEADER_DIR}/code_editor/juce_LuaCodeTokeniser.h
      ${LIB_HEADER_DIR}/code_editor/juce_XMLCodeTokeniser.h
      ${LIB_HEADER_DIR}/documents/juce_FileBasedDocument.h
      ${LIB_HEADER_DIR}/embedding/juce_ActiveXControlComponent.h
      ${LIB_HEADER_DIR}/embedding/juce_AndroidViewComponent.h
      ${LIB_HEADER_DIR}/embedding/juce_HWNDComponent.h
      ${LIB_HEADER_DIR}/embedding/juce_NSViewComponent.h
      ${LIB_HEADER_DIR}/embedding/juce_UIViewComponent.h
      ${LIB_HEADER_DIR}/embedding/juce_XEmbedComponent.h
      ${LIB_HEADER_DIR}/misc/juce_AnimatedAppComponent.h
      ${LIB_HEADER_DIR}/misc/juce_AppleRemote.h
      ${LIB_HEADER_DIR}/misc/juce_BubbleMessageComponent.h
      ${LIB_HEADER_DIR}/misc/juce_ColourSelector.h
      ${LIB_HEADER_DIR}/misc/juce_KeyMappingEditorComponent.h
      ${LIB_HEADER_DIR}/misc/juce_LiveConstantEditor.h
      ${LIB_HEADER_DIR}/misc/juce_PreferencesPanel.h
      ${LIB_HEADER_DIR}/misc/juce_PushNotifications.h
      ${LIB_HEADER_DIR}/misc/juce_RecentlyOpenedFilesList.h
      ${LIB_HEADER_DIR}/misc/juce_SplashScreen.h
      ${LIB_HEADER_DIR}/misc/juce_SystemTrayIconComponent.h
      ${LIB_HEADER_DIR}/misc/juce_WebBrowserComponent.h     
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_gui_extra.cpp
      ${LIB_SOURCE_DIR}/code_editor/juce_CodeDocument.cpp
      ${LIB_SOURCE_DIR}/code_editor/juce_CodeEditorComponent.cpp
      ${LIB_SOURCE_DIR}/code_editor/juce_CPlusPlusCodeTokeniser.cpp
      ${LIB_SOURCE_DIR}/code_editor/juce_LuaCodeTokeniser.cpp
      ${LIB_SOURCE_DIR}/code_editor/juce_XMLCodeTokeniser.cpp
      ${LIB_SOURCE_DIR}/documents/juce_FileBasedDocument.cpp
      ${LIB_SOURCE_DIR}/misc/juce_AnimatedAppComponent.cpp
      ${LIB_SOURCE_DIR}/misc/juce_BubbleMessageComponent.cpp
      ${LIB_SOURCE_DIR}/misc/juce_ColourSelector.cpp
      ${LIB_SOURCE_DIR}/misc/juce_KeyMappingEditorComponent.cpp
      ${LIB_SOURCE_DIR}/misc/juce_LiveConstantEditor.cpp
      ${LIB_SOURCE_DIR}/misc/juce_PreferencesPanel.cpp
      ${LIB_SOURCE_DIR}/misc/juce_PushNotifications.cpp
      ${LIB_SOURCE_DIR}/misc/juce_RecentlyOpenedFilesList.cpp
      ${LIB_SOURCE_DIR}/misc/juce_SplashScreen.cpp
      ${LIB_SOURCE_DIR}/misc/juce_SystemTrayIconComponent.cpp
      ${LIB_SOURCE_DIR}/misc/juce_WebBrowserComponent.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_ActiveXComponent.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_ActiveXComponent.h
      ${LIB_SOURCE_DIR}/native/juce_win32_HWNDComponent.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_HWNDComponent.h
      ${LIB_SOURCE_DIR}/native/juce_win32_SystemTrayIcon.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_SystemTrayIcon.h
      ${LIB_SOURCE_DIR}/native/juce_win32_WebBrowserComponent.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_WebBrowserComponent.h
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
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_gui_basics)

endif()

# add_subdirectory(test)

