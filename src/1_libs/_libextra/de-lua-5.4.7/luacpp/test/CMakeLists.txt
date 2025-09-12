
##########
# Examples
##########
add_executable(demo_LuaCpp_HelloWorld ${CMAKE_CURRENT_LIST_DIR}/demo_LuaCpp_HelloWorld.cpp)
target_link_libraries(demo_LuaCpp_HelloWorld de_luacpp)

add_custom_command(TARGET demo_LuaCpp_HelloWorld POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_LIST_DIR}/hello.lua 
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/demo_Lua/hello.lua
    COMMENT
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/demo_Lua/hello.lua copied to build" 
)

add_executable(demo_LuaCpp_Library ${CMAKE_CURRENT_LIST_DIR}/demo_LuaCpp_Library.cpp)
target_link_libraries(demo_LuaCpp_Library de_luacpp)

add_executable(demo_LuaCpp_MetaObject ${CMAKE_CURRENT_LIST_DIR}/demo_LuaCpp_MetaObject.cpp)
target_link_libraries(demo_LuaCpp_MetaObject de_luacpp)

