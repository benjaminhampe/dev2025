set(LIB_NAME de_nlohmann_json_schema_validator)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/nlohmann/json-schema.hpp)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/json-patch.cpp
        ${LIB_SOURCE_DIR}/json-patch.hpp
        ${LIB_SOURCE_DIR}/json-schema-draft7.json.cpp
        ${LIB_SOURCE_DIR}/json-uri.cpp
        ${LIB_SOURCE_DIR}/json-validator.cpp
        ${LIB_SOURCE_DIR}/smtp-address-validator.cpp
        ${LIB_SOURCE_DIR}/smtp-address-validator.hpp
        ${LIB_SOURCE_DIR}/string-format-check.cpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_nlohmann_json)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)
    endif()

endif()

# add_subdirectory(test)
