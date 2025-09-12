
##########
# Examples
##########
add_executable(demo_Lua_HelloWorld ${CMAKE_CURRENT_LIST_DIR}/demo_Lua_HelloWorld.cpp)
target_link_libraries(demo_Lua_HelloWorld de_luacpp)

add_executable(demo_Lua_Library ${CMAKE_CURRENT_LIST_DIR}/demo_Lua_Library.cpp)
target_link_libraries(demo_Lua_Library de_luacpp)

add_executable(demo_Lua_MetaObject ${CMAKE_CURRENT_LIST_DIR}/demo_Lua_MetaObject.cpp)
target_link_libraries(demo_Lua_MetaObject de_luacpp)

add_custom_command(TARGET demo_Lua_HelloWorld POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_LIST_DIR}/hello.lua 
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/demo_Lua/hello.lua
    COMMENT
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/demo_Lua/hello.lua copied to build" 
)
