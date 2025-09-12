set(DELIB_CPUID cpuid-cl) # We dont like to clash with system libs.
project(${DELIB_CPUID}-dev)

if (TARGET ${DELIB_CPUID})
   message("${BoldYellow}[benni] HEADER_GUARD: ${DELIB_CPUID}${ColourReset}")
else()
   enable_language(ASM_NASM)
   enable_language(RC)
   #enable_language(CXX)
   # Public Headers
   set(DELIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
   # set(DELIB_HEADERS ${DELIB_HEADER_DIR}/cputopology.h)
   file(GLOB_RECURSE DELIB_HEADERS "${DELIB_HEADER_DIR}/*.h")

   # Private resources
   set(DELIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file(GLOB_RECURSE DELIB_SOURCES_H "${DELIB_SOURCE_DIR}/*.h")
   file(GLOB_RECURSE DELIB_SOURCES_CPP "${DELIB_SOURCE_DIR}/*.cpp")
   set(DELIB_SOURCES ${DELIB_SOURCES_H} ${DELIB_SOURCES_CPP})
   list(APPEND DELIB_SOURCES ${DELIB_SOURCE_DIR}/resource.rc)
   list(APPEND DELIB_SOURCES ${DELIB_SOURCE_DIR}/get_cpuid_info.asm)

   # set(DELIB_SOURCES
      # ${DELIB_SOURCE_DIR}/cputopology.cpp
      # ${DELIB_SOURCE_DIR}/util_os.cpp
      # ${DELIB_SOURCE_DIR}/get_cpuid.asm
      # ${DELIB_SOURCE_DIR}/resource.h
      # ${DELIB_SOURCE_DIR}/resource.rc
   # )

   message("${BoldGreen}[benni] TOOL: ${DELIB_CPUID}${ColourReset}")
   add_executable(${DELIB_CPUID} "")

#    target_compile_definitions(${DELIB_CPUID} PUBLIC -DDELIB_BUILD_LIBRARY)

   # target_compile_definitions(${DELIB_CPUID} PUBLIC -DDELIB_MAJOR_VER=2)
   # target_compile_definitions(${DELIB_CPUID} PUBLIC -DDELIB_MINOR_VER=14)
   # target_compile_definitions(${DELIB_CPUID} PUBLIC -DDELIB_SUBMINOR_VER=2)
   # target_compile_definitions(${DELIB_CPUID} PUBLIC -DDELIB_PATCHLEVEL_VER=0)
   # target_compile_definitions(${DELIB_CPUID} PUBLIC -DDELIB_VERSION_ID=0x020e0200)
   # target_compile_definitions(${DELIB_CPUID} PUBLIC -DDELIB_VER="2.14.02")
   # target_compile_definitions(${DELIB_CPUID} PRIVATE -DHAVE_CONFIG_H=1)

   target_sources(${DELIB_CPUID} PUBLIC ${DELIB_HEADERS} PRIVATE ${DELIB_SOURCES})

   target_include_directories(${DELIB_CPUID} PUBLIC ${DELIB_HEADER_DIR})
   target_include_directories(${DELIB_CPUID} PRIVATE ${DELIB_SOURCE_DIR})

   # target_link_libraries(${DELIB_CPUID} PRIVATE DarkImage)
   # target_compile_options(${DELIB_CPUID} PRIVATE
                                    # -O3
                                    # -fwrapv
                                    # -U__STRICT_ANSI__
                                    # -fno-common
                                    # -Werror=attributes
                                    # -fvisibility=hidden
                                    # -W
                                    # -Wall
                                    # -pedantic
                                    # -Wc90-c99-compat
                                    # -Wno-long-long
                                    # -Wno-shift-negative-value
                                    # -Werror=implicit
                                    # -Werror=missing-braces
                                    # -Werror=return-type
                                    # -Werror=trigraphs
                                    # -Werror=pointer-arith
                                    # -Werror=missing-prototypes
                                    # -Werror=missing-declarations
                                    # #-Wno-error=comment
                                    # -Werror=vla
                                    # -Wno-pedantic
   # )

   # on Windows:
   if (WIN32)
      target_link_libraries(${DELIB_CPUID} PRIVATE comctl32 user32 gdi32 kernel32)
   # on Linux:
   else()
      target_link_libraries(${DELIB_CPUID} PRIVATE X11 Xxf86vm Xi Xcursor)
   endif()


   ### add_custom_command(TARGET foo POST_BUILD
   ###    COMMAND ${CMAKE_COMMAND} -E copy
   ###            ${CMAKE_SOURCE_DIR}/test/input.txt
   ###            ${CMAKE_CURRENT_BINARY_DIR}/input.txt)
endif()
