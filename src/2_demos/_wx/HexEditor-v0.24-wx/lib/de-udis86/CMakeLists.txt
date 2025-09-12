set(LIB_NAME de_udis86)
project(lib-${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

   # add_subdirectory(lib)
   set(UDIS_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
   set(UDIS_HEADERS 
      ${UDIS_HEADER_DIR}/udis86.h
      ${UDIS_HEADER_DIR}/extern.h
      ${UDIS_HEADER_DIR}/types.h
      ${UDIS_HEADER_DIR}/itab.h
   )
   
   set(UDIS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   set(UDIS_SOURCES 
      ${UDIS_SOURCE_DIR}/decode.h
      ${UDIS_SOURCE_DIR}/decode.c
      ${UDIS_SOURCE_DIR}/syn.h
      ${UDIS_SOURCE_DIR}/syn.c
      ${UDIS_SOURCE_DIR}/syn-att.c
      ${UDIS_SOURCE_DIR}/syn-intel.c
      ${UDIS_SOURCE_DIR}/udint.h
      ${UDIS_SOURCE_DIR}/udis86.c
      ${UDIS_SOURCE_DIR}/itab.c
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldBlue}[benni] SHARED_LIB: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldBlue}[benni] STATIC_LIB: ${LIB_NAME}${ColourReset}")
   endif()

   # target_compile_definitions(${LIB_NAME} PUBLIC -DUSE_IRRLICHT=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -D_UDIS_STATIC_LIB_=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)
   # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
   # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-function)
   # target_compile_options(${LIB_NAME} PRIVATE -Wno-maybe-uninitialized)
   # target_compile_options(${LIB_NAME} PRIVATE -Wno-strict-aliasing)
   target_include_directories(${LIB_NAME} PUBLIC ${UDIS_HEADER_DIR})
   # target_include_directories(${LIB_NAME} PUBLIC ${UDIS_HEADER_DIR}/..)
   # target_include_directories(${LIB_NAME} PRIVATE ${UDIS_SOURCE_DIR})
   target_sources(${LIB_NAME} PUBLIC ${UDIS_HEADERS} PRIVATE ${UDIS_SOURCES})
   # target_link_libraries(${LIB_NAME} PUBLIC stdc++fs)
   # target_link_libraries(${LIB_NAME} PUBLIC pthread)
   target_link_libraries(${LIB_NAME} PUBLIC m)

   # if(WIN32)
      # target_link_libraries(${LIB_NAME} PUBLIC glu32 opengl32
               # uuid ole32 oleaut32 ComCtl32
               # SetupAPI winmm user32 gdi32 kernel32 advapi32 shell32 )
   # else() # on Linux:
      # target_link_libraries(${LIB_NAME} PUBLIC GLU GL)
      # target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
   # endif()
   # target_link_libraries(${LIB_NAME} PUBLIC de_z)
   # target_link_libraries(${LIB_NAME} PUBLIC de_bzip2)
   # #target_link_libraries(${LIB_NAME} PUBLIC de_lzma)
   # target_link_libraries(${LIB_NAME} PUBLIC de_png)
   # target_link_libraries(${LIB_NAME} PUBLIC de_jpeg8d)

endif()

# add_subdirectory(test)
