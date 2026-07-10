set(LIB_NAME de_picojson_130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   message("${BoldBlue}[benni] INTERFACE_LIBRARY: ${LIB_NAME}, header only.${ColourReset}")
   add_library(${LIB_NAME} INTERFACE)
   target_compile_definitions(${LIB_NAME} INTERFACE -DHAVE_PICOJSON=1)
   target_compile_definitions(${LIB_NAME} INTERFACE -DHAVE_LIBPICOJSON=1)
   target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include) 
endif()

# add_subdirectory(test)
