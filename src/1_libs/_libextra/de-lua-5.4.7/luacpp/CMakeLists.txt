set(LIB_NAME de_luacpp)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/Lua.hpp
        ${LIB_HEADER_DIR}/LuaCpp.hpp
        ${LIB_HEADER_DIR}/LuaVersion.hpp
        ${LIB_HEADER_DIR}/LuaContext.hpp
        ${LIB_HEADER_DIR}/LuaMetaObject.hpp
        ${LIB_HEADER_DIR}/Engine/LuaState.hpp
        ${LIB_HEADER_DIR}/Engine/LuaType.hpp
        ${LIB_HEADER_DIR}/Engine/LuaTNil.hpp
        ${LIB_HEADER_DIR}/Engine/LuaTString.hpp
        ${LIB_HEADER_DIR}/Engine/LuaTNumber.hpp
        ${LIB_HEADER_DIR}/Engine/LuaTBoolean.hpp
        ${LIB_HEADER_DIR}/Engine/LuaTTable.hpp
        ${LIB_HEADER_DIR}/Engine/LuaTUserData.hpp
        ${LIB_HEADER_DIR}/Registry/LuaRegistry.hpp
        ${LIB_HEADER_DIR}/Registry/LuaCodeSnippet.hpp
        ${LIB_HEADER_DIR}/Registry/LuaCompiler.hpp
        ${LIB_HEADER_DIR}/Registry/LuaCFunction.hpp
        ${LIB_HEADER_DIR}/Registry/LuaLibrary.hpp
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/LuaContext.cpp 
        ${LIB_SOURCE_DIR}/LuaMetaObject.cpp
        ${LIB_SOURCE_DIR}/LuaVersion.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaState.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaType.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaTNil.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaTString.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaTNumber.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaTBoolean.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaTTable.cpp 
        ${LIB_SOURCE_DIR}/Engine/LuaTUserData.cpp 
        ${LIB_SOURCE_DIR}/Registry/LuaRegistry.cpp 
        ${LIB_SOURCE_DIR}/Registry/LuaCodeSnippet.cpp 
        ${LIB_SOURCE_DIR}/Registry/LuaCompiler.cpp 
        ${LIB_SOURCE_DIR}/Registry/LuaCFunction.cpp 
        ${LIB_SOURCE_DIR}/Registry/LuaLibrary.cpp 
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")	
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Engine)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Registry)
    if (MSVC)
        # set_target_properties(${LIB_NAME} PROPERTIES FOLDER _benni)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC de_lua)
    
    # file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK

    # include(GNUInstallDirs)

    # find_package(Lua REQUIRED)

    # include_directories(example_HelloLua PRIVATE ${LUA_INCLUDE_DIR})

endif()

add_subdirectory(test)
# add_library(${LIB_NAME} SHARED PUBLIC ${SOURCE_FILES})
# target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

# add_library(luacpp_static STATIC ${SOURCE_FILES})
# set_target_properties(luacpp_static PROPERTIES OUTPUT_NAME luacpp)
# target_link_libraries(luacpp PRIVATE de_lua )
# target_link_libraries(luacpp_static ${LUA_LIBRARIES})

# ##########
# # Examples
# ##########
# add_executable(example_helloworld src/Example/example_helloworld.cpp)
# target_link_libraries(example_helloworld luacpp)

# add_executable(example_LuaLibrary src/Example/example_LuaLibrary.cpp)
# target_link_libraries(example_LuaLibrary luacpp)

# add_executable(example_LuaMetaObject src/Example/example_LuaMetaObject.cpp)
# target_link_libraries(example_LuaMetaObject luacpp)

# add_custom_command(TARGET example_helloworld POST_BUILD
	# COMMAND ${CMAKE_COMMAND} -E copy 
    # ${PROJECT_SOURCE_DIR}/Example/hello.lua 
    # ${PROJECT_BINARY_DIR}/hello.lua
	# COMMENT "${PROJECT_BINARY_DIR}/hello.lua copied to build" 
# )

#########
# Install
#########
# include(CMakePackageConfigHelpers)

# set(CMAKE_CONFIG_DEST "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake")
# set(LuaCpp_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
# set(LuaCpp_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}")

# install(TARGETS luacpp
        # LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install(TARGETS luacpp_static
        # DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install(FILES LuaCpp.hpp Lua.hpp LuaContext.hpp LuaMetaObject.hpp LuaVersion.hpp 
	# DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# install(DIRECTORY ${CMAKE_SOURCE_DIR}/Registry ${CMAKE_SOURCE_DIR}/Engine
	# DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
        # FILES_MATCHING
        # PATTERN "*.hpp"
# )
# configure_package_config_file(
	# LuaCppConfig.cmake.in
	# ${CMAKE_CURRENT_BINARY_DIR}/LuaCppConfig.cmake
	# INSTALL_DESTINATION "${CMAKE_CONFIG_DEST}"
	# PATH_VARS
		# LuaCpp_INCLUDE_DIR
		# LuaCpp_INSTALL_LIBDIR
# )
# write_basic_package_version_file(
	# ${CMAKE_CURRENT_BINARY_DIR}/LuaCppConfigVersion.cmake
  	# VERSION 0.1.0
  	# COMPATIBILITY SameMajorVersion
	# ARCH_INDEPENDENT
# )
# install(FILES
	# ${CMAKE_CURRENT_BINARY_DIR}/LuaCppConfigVersion.cmake
	# ${CMAKE_CURRENT_BINARY_DIR}/LuaCppConfig.cmake
	# DESTINATION ${CMAKE_CONFIG_DEST}
# )

#########
# Testing
#########
# enable_testing()
# # Install Google test library
# set(GOOGLETEST_INSTALL "${CMAKE_CURRENT_BINARY_DIR}/googletest-install")
# set(GOOGLETEST_INCLUDE "${GOOGLETEST_INSTALL}/include")
# include(ExternalProject)
# ExternalProject_Add(googletest
  # GIT_REPOSITORY    https://github.com/google/googletest.git
  # GIT_TAG           main
  # SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
  # BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
  # CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${GOOGLETEST_INSTALL} -DINCLUDE_INSTALL_DIR=${GOOGLETEST_INSTALL}
  # TEST_COMMAND      ""
# )
# link_directories(${GOOGLETEST_INSTALL}/lib)
# include_directories(${GOOGLETEST_INSTALL}/include)
# include(GoogleTest)

# add_executable(testLuaContext UnitTest/TestLuaContext.cpp)
# add_dependencies(testLuaContext googletest)
# target_link_libraries(testLuaContext luacpp_static gtest_main gtest pthread)
# gtest_discover_tests(testLuaContext)

# add_executable(testLuaTypes UnitTest/TestLuaTypes.cpp)
# add_dependencies(testLuaTypes googletest)
# target_link_libraries(testLuaTypes luacpp_static gtest_main gtest pthread)
# gtest_discover_tests(testLuaTypes)

# add_executable(testLuaCompiler UnitTest/TestLuaCompiler.cpp)
# add_dependencies(testLuaCompiler googletest)
# target_link_libraries(testLuaCompiler luacpp_static gtest_main gtest pthread)
# gtest_discover_tests(testLuaCompiler)

# add_executable(testLuaMetaObject UnitTest/TestLuaMetaObject.cpp)
# add_dependencies(testLuaMetaObject googletest)
# target_link_libraries(testLuaMetaObject luacpp_static gtest_main gtest pthread)
# gtest_discover_tests(testLuaMetaObject)


#############
# Memory Test
#############
# find_program(valgrind valgrind)

# if (valgrind)
	# message(STATUS "Found command 'valgrand' => Use target 'test_memory' to perform memory test")
	# add_custom_target( test_memory
		# COMMAND valgrind -v --run-cxx-freeres=yes --run-libc-freeres=yes make test 
	# )
# else()
	# message(WARNING "Cannot find command 'valgrand' => Please install package 'valgrand' to run memory tests")
# endif()


##########
# Coverage
##########

# if( CMAKE_BUILD_TYPE STREQUAL "Coverage" )
    # if( NOT CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
        # message(WARNING "Coverage not yet implemented for your compiler '${CMAKE_CXX_COMPILER_ID}' (only GNU and Clang)")
        # return()
    # endif()

    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

    # find_program (gcovr gcovr)
    # find_program (lcov lcov)

    # if (gcovr)
	# message (STATUS "Found command 'gcovr' => Use target 'coverage-html' or 'coverage-cli' to generate coverage report '${CMAKE_BINARY_DIR}/gcovr.html'")
	# add_custom_target (coverage-html
	    # COMMAND make test
            # COMMAND ${gcovr} --root             ${CMAKE_SOURCE_DIR}
                             # --exclude          ${CMAKE_SOURCE_DIR}/3rdparty
			     # --exclude          ${CMAKE_SOURCE_DIR}/Example
                             # --exclude          ${CMAKE_BINARY_DIR}
                             # --object-directory ${CMAKE_BINARY_DIR}
                             # --output           ${CMAKE_BINARY_DIR}/gcovr.html
                             # --html
                             # --html-details
                             # --sort-uncovered
                             # --print-summary
                             # --exclude-unreachable-branches
			     # --exclude-throw-branches
            # COMMAND echo "To display coverage report: firefox ${CMAKE_BINARY_DIR}/gcovr.html"
         # ) 
	# add_custom_target (coverage-json
	    # COMMAND make test
            # COMMAND ${gcovr} --root             ${CMAKE_SOURCE_DIR}
                             # --exclude          ${CMAKE_SOURCE_DIR}/3rdparty
			     # --exclude          ${CMAKE_SOURCE_DIR}/Example
                             # --exclude          ${CMAKE_BINARY_DIR}
                             # --object-directory ${CMAKE_BINARY_DIR}
                             # --sort-uncovered
                             # --print-summary
                             # --exclude-unreachable-branches
			     # --exclude-throw-branches
			     # --json 
			     # --output coverallis.json
	# )
	# add_custom_target (coverage-cli
	    # COMMAND make test
            # COMMAND ${gcovr} --root             ${CMAKE_SOURCE_DIR}
                             # --exclude          ${CMAKE_SOURCE_DIR}/3rdparty
			     # --exclude          ${CMAKE_SOURCE_DIR}/Example
                             # --exclude          ${CMAKE_BINARY_DIR}
                             # --object-directory ${CMAKE_BINARY_DIR}
                             # --sort-uncovered
                             # --print-summary
                             # --exclude-unreachable-branches
			     # --exclude-throw-branches
         # ) 
    # else()
	 # message(FATAL_ERROR "Cannot find command 'gcovr' => Please install package 'gcovr' to generate code coverage report (JSON, CLI, HTML)")
    # endif()

    # if(lcov)
	# add_custom_target (coverage-lcov
	    # COMMAND make test
	    # COMMAND echo "======= ${PROJECT_DIR} =============="
            # COMMAND ${lcov}  --capture
	                     # --directory        ${CMAKE_BINARY_DIR}
			     # --base-directory   ${CMAKE_SOURCE_DIR}
                             # --output-file      ${CMAKE_BINARY_DIR}/coverage.info
			     # --rc               lcov_branch_coverage=1
            # COMMAND ${lcov}  --remove ${CMAKE_BINARY_DIR}/coverage.info '/usr/include/*' 
	                     # --output ${CMAKE_BINARY_DIR}/coverage.info
            # COMMAND ${lcov}  --remove ${CMAKE_BINARY_DIR}/coverage.info '*/build/googletest-install/*' 
	                     # --output ${CMAKE_BINARY_DIR}/coverage.info
            # COMMAND ${lcov}  --list   ${CMAKE_BINARY_DIR}/coverage.info 
         # ) 
    # else()
	    # message(WARNING "Cannot find command 'lcov' => Please install package 'lcov' to generate code coverage report (Coveralls)")
    # endif()
# endif()


###########
# Documents
###########
# option(BUILD_DOC "Build documentation" OFF)

# check if Doxygen is installed
# find_package(Doxygen)
# if (DOXYGEN_FOUND)
    # # set input and output files
    # set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/../docs/Doxyfile.in)
    # set(DOXYGEN_OUT ${PROJECT_BINARY_DIR}/Doxyfile)

    # # request to configure the file
    # configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    # message("Doxygen build started")

    # # note the option ALL which allows to build the docs together with the application
    # add_custom_target( doc_doxygen
        # COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	# WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        # COMMENT "Generating API documentation with Doxygen"
        # VERBATIM )
    # add_custom_target( doc_pdf
	# COMMAND cd doc_doxygen/latex && make pdf
	# COMMAND mv doc_doxygen/latex/refman.pdf ${CMAKE_BINARY_DIR}/luacpp.pdf
	# DEPENDS doc_doxygen
    # )
# else (DOXYGEN_FOUND)
  # message("Doxygen need to be installed to generate the doxygen documentation")
# endif (DOXYGEN_FOUND)

