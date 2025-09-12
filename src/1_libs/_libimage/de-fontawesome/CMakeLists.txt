### FontAwesome 463
set(LIB_NAME de_fontawesome)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/de_fontawesome.hpp)
    set(LIB_SOURCES ${LIB_SOURCE_DIR}/de_fontawesome.cpp)

    add_library(${LIB_NAME} STATIC "")

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_FONTAWESOME=1)

    # copy fonts:
    # file(COPY ${CMAKE_CURRENT_LIST_DIR}/fontawesome463.ttf DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # file(COPY ${CMAKE_CURRENT_LIST_DIR}/la-brands-400.ttf DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # file(COPY ${CMAKE_CURRENT_LIST_DIR}/la-regular-400.ttf DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # file(COPY ${CMAKE_CURRENT_LIST_DIR}/la-solid-900.ttf DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

   # add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy fontawesome.ttf" COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/fontawesome.ttf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fontawesome.ttf)

#   add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy fontawesome463.ttf" COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_LIST_DIR}/fontawesome463.ttf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fontawesome463.ttf)

#   add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy la-brands-400.ttf" COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_LIST_DIR}/la-brands-400.ttf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-brands-400.ttf)

#   add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy la-regular-400.ttf" COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_LIST_DIR}/la-regular-400.ttf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-regular-400.ttf)

#   add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy la-solid-900.ttf" COMMAND ${CMAKE_COMMAND} -E copy
#         ${CMAKE_CURRENT_LIST_DIR}/la-solid-900.ttf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-solid-900.ttf)
endif()


# file(COPY "${BHD}/Qt5Core.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release
# file(COPY "${BHD}/Qt5Gui.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release
# file(COPY "${BHD}/Qt5Widgets.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release
# file(COPY "${BHD}/Qt5WinExtras.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release
