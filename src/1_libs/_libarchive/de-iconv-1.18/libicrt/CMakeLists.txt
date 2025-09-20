set(LIB_NAME de_iconv_icrt) # We dont like to clash with system libs.

project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	# if (WIN32)
	# enable_language(RC)
	# # list(APPEND LIB_SOURCES 
	# # ${LIB_SOURCE_DIR}/common/win32thread.c
	# # ${LIB_SOURCE_DIR}/common/win32thread.h )
	# endif ()

	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_HEADERS
        ${LIB_HEADER_DIR}/binary-io.h
        ${LIB_HEADER_DIR}/progname.h
        ${LIB_HEADER_DIR}/relocatable.h
        ${LIB_HEADER_DIR}/safe-read.h
        ${LIB_HEADER_DIR}/xalloc.h
        # ${LIB_HEADER_DIR}/alloca.h
        ${LIB_HEADER_DIR}/uniwidth.h
        ${LIB_HEADER_DIR}/uniwidth/cjk.h                
        
        # ${LIB_HEADER_DIR}/alloca.in.h
        # ${LIB_HEADER_DIR}/allocator.h
        # ${LIB_HEADER_DIR}/areadlink.h
        # ${LIB_HEADER_DIR}/arg-nonnull.h
        # ${LIB_HEADER_DIR}/assert.in.h
        # ${LIB_HEADER_DIR}/basename-lgpl.h

        # ${LIB_HEADER_DIR}/c++defs.h
        # ${LIB_HEADER_DIR}/c-ctype.h
        # ${LIB_HEADER_DIR}/careadlinkat.h
        # ${LIB_HEADER_DIR}/cdefs.h
        # ${LIB_HEADER_DIR}/cloexec.h
        # ${LIB_HEADER_DIR}/eloop-threshold.h
        # ${LIB_HEADER_DIR}/errno.h
        # ${LIB_HEADER_DIR}/errno.in.h
        # ${LIB_HEADER_DIR}/error.h
        # ${LIB_HEADER_DIR}/error.in.h
        # ${LIB_HEADER_DIR}/fcntl.h
        # ${LIB_HEADER_DIR}/fcntl.in.h
        # ${LIB_HEADER_DIR}/fd-hook.h
        # ${LIB_HEADER_DIR}/filename.h
        # ${LIB_HEADER_DIR}/getprogname.h
        # ${LIB_HEADER_DIR}/gettext.h
        # ${LIB_HEADER_DIR}/idx.h
        # ${LIB_HEADER_DIR}/intprops-internal.h
        # ${LIB_HEADER_DIR}/intprops.h
        # ${LIB_HEADER_DIR}/inttypes.h
        # ${LIB_HEADER_DIR}/inttypes.in.h
        # ${LIB_HEADER_DIR}/libc-config.h
        # ${LIB_HEADER_DIR}/limits.h
        # ${LIB_HEADER_DIR}/limits.in.h
        # ${LIB_HEADER_DIR}/localcharset.h
        # ${LIB_HEADER_DIR}/malloc
        # ${LIB_HEADER_DIR}/malloc/scratch_buffer.gl.h
        # ${LIB_HEADER_DIR}/malloc/scratch_buffer.h
        # ${LIB_HEADER_DIR}/malloca.h
        # ${LIB_HEADER_DIR}/minmax.h
        # ${LIB_HEADER_DIR}/msvc-inval.h
        # ${LIB_HEADER_DIR}/msvc-nothrow.h
        # ${LIB_HEADER_DIR}/pathmax.h
        # ${LIB_HEADER_DIR}/scratch_buffer.h
        # ${LIB_HEADER_DIR}/signal.h
        # ${LIB_HEADER_DIR}/signal.in.h
        # ${LIB_HEADER_DIR}/stat-time.h
        # ${LIB_HEADER_DIR}/stat-w32.h
        # ${LIB_HEADER_DIR}/stdckdint.h
        # ${LIB_HEADER_DIR}/stdckdint.in.h
        # ${LIB_HEADER_DIR}/stddef.h
        # ${LIB_HEADER_DIR}/stddef.in.h
        # ${LIB_HEADER_DIR}/stdint.h
        # ${LIB_HEADER_DIR}/stdint.in.h
        # ${LIB_HEADER_DIR}/stdio.h
        # ${LIB_HEADER_DIR}/stdio.in.h
        # ${LIB_HEADER_DIR}/stdlib.h
        # ${LIB_HEADER_DIR}/stdlib.in.h
        # ${LIB_HEADER_DIR}/streq.h
        # ${LIB_HEADER_DIR}/strerror-override.h
        # ${LIB_HEADER_DIR}/string.h
        # ${LIB_HEADER_DIR}/string.in.h
        # ${LIB_HEADER_DIR}/sys
        # ${LIB_HEADER_DIR}/sys/stat.h
        # ${LIB_HEADER_DIR}/sys/types.h
        # ${LIB_HEADER_DIR}/sys-limits.h
        # ${LIB_HEADER_DIR}/sys_stat.in.h
        # ${LIB_HEADER_DIR}/sys_types.in.h
        # ${LIB_HEADER_DIR}/time.h
        # ${LIB_HEADER_DIR}/time.in.h
        # ${LIB_HEADER_DIR}/unictype
        # ${LIB_HEADER_DIR}/unictype/bitmap.h
        # ${LIB_HEADER_DIR}/unistd.h
        # ${LIB_HEADER_DIR}/unistd.in.h
        # ${LIB_HEADER_DIR}/unitypes.h
        # ${LIB_HEADER_DIR}/unitypes.in.h
        # ${LIB_HEADER_DIR}/uniwidth

        # ${LIB_HEADER_DIR}/uniwidth/width0.h
        # ${LIB_HEADER_DIR}/uniwidth/width2.h
        
        # ${LIB_HEADER_DIR}/uniwidth.in.h
        # ${LIB_HEADER_DIR}/unlocked-io.h
        # ${LIB_HEADER_DIR}/verify.h
        # ${LIB_HEADER_DIR}/warn-on-use.h
        # ${LIB_HEADER_DIR}/wchar.h
        # ${LIB_HEADER_DIR}/wchar.in.h
        # ${LIB_HEADER_DIR}/xalloc-oversized.h

        # ${LIB_HEADER_DIR}/xreadlink.h
        # ${LIB_HEADER_DIR}/_Noreturn.h
	)
	
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
	set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/binary-io.c
        ${LIB_SOURCE_DIR}/progname.c
        ${LIB_SOURCE_DIR}/relocatable.c
        ${LIB_SOURCE_DIR}/safe-read.c
        ${LIB_SOURCE_DIR}/xmalloc.c    
        ${LIB_SOURCE_DIR}/msvc-inval.h
        ${LIB_SOURCE_DIR}/msvc-inval.c
        ${LIB_SOURCE_DIR}/msvc-nothrow.h
        ${LIB_SOURCE_DIR}/msvc-nothrow.c                
        ${LIB_SOURCE_DIR}/read.c
        ${LIB_SOURCE_DIR}/unistd.c
        ${LIB_SOURCE_DIR}/uniwidth/width.c


#               ${LIB_SOURCE_DIR}/allocator.c
        # ${LIB_SOURCE_DIR}/areadlink.c
        # ${LIB_SOURCE_DIR}/basename-lgpl.c
        # ${LIB_SOURCE_DIR}/c-ctype.c
        # ${LIB_SOURCE_DIR}/canonicalize-lgpl.c
        # ${LIB_SOURCE_DIR}/careadlinkat.c
        # ${LIB_SOURCE_DIR}/cloexec.c
        # ${LIB_SOURCE_DIR}/close.c
        # ${LIB_SOURCE_DIR}/dup2.c
        # ${LIB_SOURCE_DIR}/error.c
        # ${LIB_SOURCE_DIR}/fcntl.c
        # ${LIB_SOURCE_DIR}/fd-hook.c
        # ${LIB_SOURCE_DIR}/free.c
        # ${LIB_SOURCE_DIR}/fstat.c
        # ${LIB_SOURCE_DIR}/getdtablesize.c
        # ${LIB_SOURCE_DIR}/getprogname.c
        # ${LIB_SOURCE_DIR}/malloc/scratch_buffer_grow.c
        # ${LIB_SOURCE_DIR}/malloc/scratch_buffer_grow_preserve.c
        # ${LIB_SOURCE_DIR}/malloc/scratch_buffer_set_array_size.c
        # ${LIB_SOURCE_DIR}/malloc.c
        # ${LIB_SOURCE_DIR}/malloca.c
        # ${LIB_SOURCE_DIR}/mempcpy.c
        # # ${LIB_SOURCE_DIR}/msvc-inval.c
        # # ${LIB_SOURCE_DIR}/msvc-nothrow.c
        # ${LIB_SOURCE_DIR}/open.c
        # ${LIB_SOURCE_DIR}/progreloc.c
        # ${LIB_SOURCE_DIR}/raise.c
        # ${LIB_SOURCE_DIR}/rawmemchr.c
        # ${LIB_SOURCE_DIR}/read.c
        # ${LIB_SOURCE_DIR}/readlink.c
        # ${LIB_SOURCE_DIR}/realloc.c
#               # ${LIB_SOURCE_DIR}/relocwrapper.c # bad LIBDIRS
        # ${LIB_SOURCE_DIR}/setenv.c
        # ${LIB_SOURCE_DIR}/sigprocmask.c
        # ${LIB_SOURCE_DIR}/stat-time.c
        # ${LIB_SOURCE_DIR}/stat-w32.c
        # ${LIB_SOURCE_DIR}/stat.c
        # ${LIB_SOURCE_DIR}/stdio-read.c
        # ${LIB_SOURCE_DIR}/stdio-write.c
        # ${LIB_SOURCE_DIR}/stdlib.c
        # ${LIB_SOURCE_DIR}/strerror-override.c
        # ${LIB_SOURCE_DIR}/strerror.c

        # ${LIB_SOURCE_DIR}/xreadlink.c
        # ${LIB_SOURCE_DIR}/xstrdup.c
	)

	# if (BUILD_SHARED)
		# message("${BoldCyan}SHARED_LIBRARY: ${LIB_NAME}${ColourReset}")
		# add_library(${LIB_NAME} SHARED "")
	# else()
	message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
	add_library(${LIB_NAME} STATIC "")
	# endif()

	# target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBCHARSET=1)
	# target_compile_definitions(${LIB_NAME} PUBLIC BUILDING_LIBCHARSET=1)
	target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
        target_compile_definitions(${LIB_NAME} PRIVATE EXEEXT=".exe")
		
	target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/malloc)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/sys)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/unictype)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/uniwidth)

    target_link_libraries(${LIB_NAME} PUBLIC de_iconv_charset)
	# target_link_libraries(${LIB_NAME} PUBLIC m)

endif()
