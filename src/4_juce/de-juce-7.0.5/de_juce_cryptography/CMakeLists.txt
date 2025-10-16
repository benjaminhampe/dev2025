set(LIB_NAME de_juce_cryptography)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_cryptography)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_cryptography)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_cryptography.h
      ${LIB_HEADER_DIR}/juce_cryptography_config.h
      ${LIB_HEADER_DIR}/encryption/juce_BlowFish.h
      ${LIB_HEADER_DIR}/encryption/juce_Primes.h
      ${LIB_HEADER_DIR}/encryption/juce_RSAKey.h
      ${LIB_HEADER_DIR}/hashing/juce_MD5.h
      ${LIB_HEADER_DIR}/hashing/juce_SHA256.h
      ${LIB_HEADER_DIR}/hashing/juce_Whirlpool.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_cryptography.cpp
      ${LIB_SOURCE_DIR}/encryption/juce_BlowFish.cpp
      ${LIB_SOURCE_DIR}/encryption/juce_Primes.cpp
      ${LIB_SOURCE_DIR}/encryption/juce_RSAKey.cpp
      ${LIB_SOURCE_DIR}/hashing/juce_MD5.cpp
      ${LIB_SOURCE_DIR}/hashing/juce_SHA256.cpp
      ${LIB_SOURCE_DIR}/hashing/juce_Whirlpool.cpp
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

