set(LIB_NAME de_juce_data_structures)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_data_structures)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_data_structures)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_data_structures.h
      ${LIB_HEADER_DIR}/juce_data_structures_config.h
      ${LIB_HEADER_DIR}/app_properties/juce_ApplicationProperties.h
      ${LIB_HEADER_DIR}/app_properties/juce_PropertiesFile.h
      ${LIB_HEADER_DIR}/undomanager/juce_UndoableAction.h
      ${LIB_HEADER_DIR}/undomanager/juce_UndoManager.h
      ${LIB_HEADER_DIR}/values/juce_CachedValue.h
      ${LIB_HEADER_DIR}/values/juce_Value.h
      ${LIB_HEADER_DIR}/values/juce_ValueTree.h
      ${LIB_HEADER_DIR}/values/juce_ValueTreePropertyWithDefault.h
      ${LIB_HEADER_DIR}/values/juce_ValueTreeSynchroniser.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_data_structures.cpp     
      ${LIB_SOURCE_DIR}/app_properties/juce_ApplicationProperties.cpp
      ${LIB_SOURCE_DIR}/app_properties/juce_PropertiesFile.cpp
      ${LIB_SOURCE_DIR}/undomanager/juce_UndoableAction.cpp
      ${LIB_SOURCE_DIR}/undomanager/juce_UndoManager.cpp
      ${LIB_SOURCE_DIR}/values/juce_CachedValue.cpp
      ${LIB_SOURCE_DIR}/values/juce_Value.cpp
      ${LIB_SOURCE_DIR}/values/juce_ValueTree.cpp
      ${LIB_SOURCE_DIR}/values/juce_ValueTreePropertyWithDefault_test.cpp
      ${LIB_SOURCE_DIR}/values/juce_ValueTreeSynchroniser.cpp
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

endif()

# add_subdirectory(test)

