set(LIB_GLUS de_glus)
project(${LIB_GLUS}-dev C)

if (TARGET ${LIB_GLUS})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_GLUS}${ColourReset}")
else()
   # set(NAME_DEBUG_MODE 0)
   # if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      # set(GLUS_DEBUG_MODE 1)
   # endif()

   set(GLUS_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
   set(GLUS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

   file(GLOB_RECURSE GLUS_HEADERS_H    "${GLUS_HEADER_DIR}/*.h")
   file(GLOB_RECURSE GLUS_HEADERS_HPP  "${GLUS_HEADER_DIR}/*.hpp")   
   
   set(GLUS_HEADERS ${GLUS_HEADERS_H} ${GLUS_HEADERS_HPP})
   
   file(GLOB_RECURSE GLUS_SOURCES_C    "${GLUS_SOURCE_DIR}/*.c")
   file(GLOB_RECURSE GLUS_SOURCES_CPP  "${GLUS_SOURCE_DIR}/*.cpp")
   file(GLOB_RECURSE GLUS_SOURCES_H    "${GLUS_SOURCE_DIR}/*.h")
   file(GLOB_RECURSE GLUS_SOURCES_HPP  "${GLUS_SOURCE_DIR}/*.hpp")   
   
   set(GLUS_SOURCES ${GLUS_SOURCES_C} ${GLUS_SOURCES_CPP} ${GLUS_SOURCES_H} ${GLUS_SOURCES_HPP})

#   if (WIN32)
#      enable_language(RC)
#      list(APPEND GLUS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
#      list(APPEND GLUS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
#   endif()
#
   if (BUILD_SHARED)
      set(BUILD_SHARED_LIBS 1)
      message("${BoldBlue}[benni] SHARED_LIBRARY: ${LIB_GLUS}${ColourReset}")
      add_library(${LIB_GLUS} SHARED "")
      #target_compile_definitions(${LIB_GLUS} PUBLIC -DGLUS_DLL=1)
     else()
      message("${BoldBlue}[benni] STATIC_LIBRARY: ${LIB_GLUS}${ColourReset}")
      add_library(${LIB_GLUS} STATIC "")
      target_compile_definitions(${LIB_GLUS} PUBLIC -DGLUS_STATIC=1)
   endif()

   # target_compile_definitions(${LIB_GLUS} PUBLIC -DBENNI_USE_GLEW=1)
   # target_compile_definitions(${LIB_GLUS} PUBLIC -DGLUS_BUILD=1)
   # target_compile_definitions(${LIB_GLUS} PUBLIC -DGLUS_NO_GLU=1)
   #target_compile_definitions(${LIB_GLUS} PUBLIC -DGLUS_EGL=1)

   target_sources(${LIB_GLUS} PUBLIC ${GLUS_HEADERS} PRIVATE ${GLUS_SOURCES})
   target_include_directories(${LIB_GLUS} PUBLIC ${GLUS_HEADER_DIR})
   target_include_directories(${LIB_GLUS} PRIVATE ${GLUS_SOURCE_DIR})
   
   #target_compile_options(${LIB_GLUS} PRIVATE -fno-builtin -fno-stack-protector)
   #target_link_libraries(${LIB_GLUS} LINK_PRIVATE -nostdlib)

   target_link_libraries(${LIB_GLUS} PUBLIC de_glew)   
   target_link_libraries(${LIB_GLUS} PUBLIC de_glfw_glew)

endif()

# add_subdirectory(test)

## Packaging
#set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
#set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The FreeType font rendering library.")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/docs/LICENSE.TXT")

#set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
#set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
#set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
#set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

#if (WIN32)
#  set(CPACK_GENERATOR ZIP)
#else()
#  set(CPACK_GENERATOR TGZ)
#endif()

#set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
#set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C/C++ Headers")
#set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION
#  "Library used to build programs which use FreeType")
#set(CPACK_COMPONENT_HEADERS_DESCRIPTION
#  "C/C++ header files for use with FreeType")
#set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)
#set(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
#set(CPACK_COMPONENT_HEADERS_GROUP "Development")

#include(CPack)
