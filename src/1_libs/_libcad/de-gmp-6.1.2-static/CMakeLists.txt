set(LIB_NAME de_gmp_static_612)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_IMPORT_DIR "${CMAKE_CURRENT_LIST_DIR}/lib")

    set(LIB_HEADER_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
	set(LIB_HEADERS "${LIB_HEADER_DIR}/gmp.h")


    message("${BoldYellow}[benni] IMP: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} STATIC IMPORTED GLOBAL)

    # target_include_directories(${LIB_NAME} INTERFACE "${LIB_HEADER_DIR}")

    # set_target_properties(${LIB_NAME} PROPERTIES
    #     IMPORTED_LOCATION_DEBUG "/path/to/libgmp_debug.a"
    #     IMPORTED_LOCATION_RELEASE "/path/to/libgmp_release.a"
    #     IMPORTED_CONFIGURATIONS "Debug;Release"
    # )

    set_target_properties(${LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${LIB_IMPORT_DIR}/libgmp.a"
        INTERFACE_INCLUDE_DIRECTORIES "${LIB_HEADER_DIR}"
        IMPORTED_CONFIGURATIONS "Release"
    )

    # set_property(TARGET ${LIB_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${LIB_HEADER_DIR}")
    
    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libcad)
    else()  
        # target_link_directories(${LIB_NAME} INTERFACE "${LIB_STATIC_DIR}")
        
        # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_STATIC_DIR}/libgmp.a")
        
        # target_link_libraries(${LIB_NAME} INTERFACE "${LIB_STATIC_DIR}/libgmp.a")
    endif()    

endif()

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD
   # COMMAND "echo" "Copying fontawesome.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/fontawesome.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fontawesome.ttf)

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD
   # COMMAND "echo" "Copying fontawesome CheatSheet.pdf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/fontawesome.pdf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fontawesome.pdf)

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD 
   # COMMAND "echo" "Copying la-regular-400.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/la-regular-400.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-regular-400.ttf)

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD 
   # COMMAND "echo" "Copying la-brands-400.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/la-brands-400.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-brands-400.ttf)

         
# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD 
   # COMMAND "echo" "Copying la-solid-900.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/la-solid-900.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-solid-900.ttf)
