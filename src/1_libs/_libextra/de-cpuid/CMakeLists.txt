set(DELIB_CPUID de_cpuid) # We dont like to clash with system libs.
project(${DELIB_CPUID}-dev)

if (TARGET ${DELIB_CPUID})
   message("${BoldYellow}[benni] HEADER_GUARD: ${DELIB_CPUID}${ColourReset}")
else()
   set(DELIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
   file(GLOB_RECURSE DELIB_HEADERS "${DELIB_HEADER_DIR}/*.hpp")

   set(DELIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   if (WIN32)
      set(DELIB_SOURCES
         ${DELIB_SOURCE_DIR}/de_cpuid.cpp
         ${DELIB_SOURCE_DIR}/CPUInfo.cpp
         ${DELIB_SOURCE_DIR}/win/de_win32api.hpp
         ${DELIB_SOURCE_DIR}/win/de_cpu_get_count_win.cpp
         ${DELIB_SOURCE_DIR}/win/de_cpu_get_speed_mhz_win.cpp
         ${DELIB_SOURCE_DIR}/win/de_cpu_get_usage_win.cpp
         ${DELIB_SOURCE_DIR}/win/other.cpp)
   else()
      set(DELIB_SOURCES
         ${DELIB_SOURCE_DIR}/de_cpuid.cpp
         ${DELIB_SOURCE_DIR}/CPUInfo.cpp
         ${DELIB_SOURCE_DIR}/linux/de_cpu_get_count_x11.cpp
         ${DELIB_SOURCE_DIR}/linux/de_cpu_get_speed_mhz_x11.cpp
         ${DELIB_SOURCE_DIR}/linux/de_cpu_get_usage_x11.cpp)
   endif()

   message("${BoldGreen}[benni] TOOL: ${DELIB_CPUID}${ColourReset}")
   add_library(${DELIB_CPUID} "")

   target_sources(${DELIB_CPUID} PUBLIC ${DELIB_HEADERS} PRIVATE ${DELIB_SOURCES})

   target_include_directories(${DELIB_CPUID} PUBLIC ${DELIB_HEADER_DIR})
   target_include_directories(${DELIB_CPUID} PRIVATE ${DELIB_SOURCE_DIR})

   if (WIN32)
      target_link_libraries(${DELIB_CPUID} PRIVATE comctl32 user32 gdi32 kernel32)
   else()
      target_link_libraries(${DELIB_CPUID} PRIVATE X11 Xxf86vm Xi Xcursor)
   endif()


   ### add_custom_command(TARGET foo POST_BUILD
   ###    COMMAND ${CMAKE_COMMAND} -E copy
   ###            ${CMAKE_SOURCE_DIR}/test/input.txt
   ###            ${CMAKE_CURRENT_BINARY_DIR}/input.txt)
endif()

add_subdirectory(test)
