set(LIB_NAME de_cv_ade) # Another Dependency Engine, GraphLib
project(${LIB_NAME}-0.1.2e)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/ade/edge.hpp
        ${LIB_HEADER_DIR}/ade/graph.hpp
        ${LIB_HEADER_DIR}/ade/graph_listener.hpp
        ${LIB_HEADER_DIR}/ade/handle.hpp
        ${LIB_HEADER_DIR}/ade/node.hpp
        ${LIB_HEADER_DIR}/ade/passmanager.hpp
        ${LIB_HEADER_DIR}/ade/typed_graph.hpp
        ${LIB_HEADER_DIR}/ade/typed_metadata.hpp
        ${LIB_HEADER_DIR}/ade/communication/callback_connector.hpp
        ${LIB_HEADER_DIR}/ade/communication/comm_buffer.hpp
        ${LIB_HEADER_DIR}/ade/communication/comm_interface.hpp
        ${LIB_HEADER_DIR}/ade/execution_engine/backend.hpp
        ${LIB_HEADER_DIR}/ade/execution_engine/executable.hpp
        ${LIB_HEADER_DIR}/ade/execution_engine/execution_engine.hpp
        ${LIB_HEADER_DIR}/ade/helpers/search.hpp
        ${LIB_HEADER_DIR}/ade/helpers/subgraphs.hpp
        ${LIB_HEADER_DIR}/ade/memory/alloc.hpp
        ${LIB_HEADER_DIR}/ade/memory/memory_accessor.hpp
        ${LIB_HEADER_DIR}/ade/memory/memory_access_listener.hpp
        ${LIB_HEADER_DIR}/ade/memory/memory_descriptor.hpp
        ${LIB_HEADER_DIR}/ade/memory/memory_descriptor_ref.hpp
        ${LIB_HEADER_DIR}/ade/memory/memory_descriptor_view.hpp
        ${LIB_HEADER_DIR}/ade/memory/memory_types.hpp
        ${LIB_HEADER_DIR}/ade/metatypes/metatypes.hpp
        ${LIB_HEADER_DIR}/ade/passes/check_cycles.hpp
        ${LIB_HEADER_DIR}/ade/passes/communications.hpp
        ${LIB_HEADER_DIR}/ade/passes/pass_base.hpp
        ${LIB_HEADER_DIR}/ade/passes/topological_sort.hpp
        ${LIB_HEADER_DIR}/ade/util/algorithm.hpp
        ${LIB_HEADER_DIR}/ade/util/assert.hpp
        ${LIB_HEADER_DIR}/ade/util/chain_range.hpp
        ${LIB_HEADER_DIR}/ade/util/checked_cast.hpp
        ${LIB_HEADER_DIR}/ade/util/container_helper.hpp
        ${LIB_HEADER_DIR}/ade/util/filter_range.hpp
        ${LIB_HEADER_DIR}/ade/util/func_ref.hpp
        ${LIB_HEADER_DIR}/ade/util/hash.hpp
        ${LIB_HEADER_DIR}/ade/util/intrusive_list.hpp
        ${LIB_HEADER_DIR}/ade/util/iota_range.hpp
        ${LIB_HEADER_DIR}/ade/util/map_range.hpp
        ${LIB_HEADER_DIR}/ade/util/math.hpp
        ${LIB_HEADER_DIR}/ade/util/md_cast.hpp
        ${LIB_HEADER_DIR}/ade/util/md_io.hpp
        ${LIB_HEADER_DIR}/ade/util/md_size.hpp
        ${LIB_HEADER_DIR}/ade/util/md_span.hpp
        ${LIB_HEADER_DIR}/ade/util/md_view.hpp
        ${LIB_HEADER_DIR}/ade/util/memory_range.hpp
        ${LIB_HEADER_DIR}/ade/util/range.hpp
        ${LIB_HEADER_DIR}/ade/util/range_iterator.hpp
        ${LIB_HEADER_DIR}/ade/util/tuple.hpp
        ${LIB_HEADER_DIR}/ade/util/type_traits.hpp
        ${LIB_HEADER_DIR}/ade/util/zip_range.hpp
    )
	
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/alloc.cpp
        ${LIB_SOURCE_DIR}/assert.cpp
        ${LIB_SOURCE_DIR}/check_cycles.cpp
        ${LIB_SOURCE_DIR}/edge.cpp
        ${LIB_SOURCE_DIR}/execution_engine.cpp
        ${LIB_SOURCE_DIR}/graph.cpp
        ${LIB_SOURCE_DIR}/memory_accessor.cpp
        ${LIB_SOURCE_DIR}/memory_descriptor.cpp
        ${LIB_SOURCE_DIR}/memory_descriptor_ref.cpp
        ${LIB_SOURCE_DIR}/memory_descriptor_view.cpp
        ${LIB_SOURCE_DIR}/metadata.cpp
        ${LIB_SOURCE_DIR}/metatypes.cpp
        ${LIB_SOURCE_DIR}/node.cpp
        ${LIB_SOURCE_DIR}/search.cpp
        ${LIB_SOURCE_DIR}/subgraphs.cpp
        ${LIB_SOURCE_DIR}/topological_sort.cpp
        ${LIB_SOURCE_DIR}/passes/communications.cpp
    )
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
		# target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/mini-gmp)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
	# target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GMP_MINI=1)
	
    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
	# target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
	# target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)
		
	# check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

	# target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)
    
	# target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

endif()

# add_subdirectory(test)
