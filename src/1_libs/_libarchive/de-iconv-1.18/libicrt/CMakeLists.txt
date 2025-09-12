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
                ${LIB_HEADER_DIR}/srclib/binary-io.h
                ${LIB_HEADER_DIR}/srclib/alloca.h
                # ${LIB_HEADER_DIR}/srclib/alloca.in.h
                # ${LIB_HEADER_DIR}/srclib/allocator.h
                # ${LIB_HEADER_DIR}/srclib/areadlink.h
                # ${LIB_HEADER_DIR}/srclib/arg-nonnull.h
                # ${LIB_HEADER_DIR}/srclib/assert.in.h
                # ${LIB_HEADER_DIR}/srclib/basename-lgpl.h

                # ${LIB_HEADER_DIR}/srclib/c++defs.h
                # ${LIB_HEADER_DIR}/srclib/c-ctype.h
                # ${LIB_HEADER_DIR}/srclib/careadlinkat.h
                # ${LIB_HEADER_DIR}/srclib/cdefs.h
                # ${LIB_HEADER_DIR}/srclib/cloexec.h
                # ${LIB_HEADER_DIR}/srclib/eloop-threshold.h
                # ${LIB_HEADER_DIR}/srclib/errno.h
                # ${LIB_HEADER_DIR}/srclib/errno.in.h
                # ${LIB_HEADER_DIR}/srclib/error.h
                # ${LIB_HEADER_DIR}/srclib/error.in.h
                # ${LIB_HEADER_DIR}/srclib/fcntl.h
                # ${LIB_HEADER_DIR}/srclib/fcntl.in.h
                # ${LIB_HEADER_DIR}/srclib/fd-hook.h
                # ${LIB_HEADER_DIR}/srclib/filename.h
                # ${LIB_HEADER_DIR}/srclib/getprogname.h
                # ${LIB_HEADER_DIR}/srclib/gettext.h
                # ${LIB_HEADER_DIR}/srclib/idx.h
                # ${LIB_HEADER_DIR}/srclib/intprops-internal.h
                # ${LIB_HEADER_DIR}/srclib/intprops.h
                # ${LIB_HEADER_DIR}/srclib/inttypes.h
                # ${LIB_HEADER_DIR}/srclib/inttypes.in.h
                # ${LIB_HEADER_DIR}/srclib/libc-config.h
                # ${LIB_HEADER_DIR}/srclib/limits.h
                # ${LIB_HEADER_DIR}/srclib/limits.in.h
                # ${LIB_HEADER_DIR}/srclib/localcharset.h
                # ${LIB_HEADER_DIR}/srclib/malloc
                # ${LIB_HEADER_DIR}/srclib/malloc/scratch_buffer.gl.h
                # ${LIB_HEADER_DIR}/srclib/malloc/scratch_buffer.h
                # ${LIB_HEADER_DIR}/srclib/malloca.h
                # ${LIB_HEADER_DIR}/srclib/minmax.h
                # ${LIB_HEADER_DIR}/srclib/msvc-inval.h
                # ${LIB_HEADER_DIR}/srclib/msvc-nothrow.h
                # ${LIB_HEADER_DIR}/srclib/pathmax.h
                # ${LIB_HEADER_DIR}/srclib/progname.h
                # ${LIB_HEADER_DIR}/srclib/relocatable.h
                # ${LIB_HEADER_DIR}/srclib/safe-read.h
                # ${LIB_HEADER_DIR}/srclib/scratch_buffer.h
                # ${LIB_HEADER_DIR}/srclib/signal.h
                # ${LIB_HEADER_DIR}/srclib/signal.in.h
                # ${LIB_HEADER_DIR}/srclib/stat-time.h
                # ${LIB_HEADER_DIR}/srclib/stat-w32.h
                # ${LIB_HEADER_DIR}/srclib/stdckdint.h
                # ${LIB_HEADER_DIR}/srclib/stdckdint.in.h
                # ${LIB_HEADER_DIR}/srclib/stddef.h
                # ${LIB_HEADER_DIR}/srclib/stddef.in.h
                # ${LIB_HEADER_DIR}/srclib/stdint.h
                # ${LIB_HEADER_DIR}/srclib/stdint.in.h
                # ${LIB_HEADER_DIR}/srclib/stdio.h
                # ${LIB_HEADER_DIR}/srclib/stdio.in.h
                # ${LIB_HEADER_DIR}/srclib/stdlib.h
                # ${LIB_HEADER_DIR}/srclib/stdlib.in.h
                # ${LIB_HEADER_DIR}/srclib/streq.h
                # ${LIB_HEADER_DIR}/srclib/strerror-override.h
                # ${LIB_HEADER_DIR}/srclib/string.h
                # ${LIB_HEADER_DIR}/srclib/string.in.h
                # ${LIB_HEADER_DIR}/srclib/sys
                # ${LIB_HEADER_DIR}/srclib/sys/stat.h
                # ${LIB_HEADER_DIR}/srclib/sys/types.h
                # ${LIB_HEADER_DIR}/srclib/sys-limits.h
                # ${LIB_HEADER_DIR}/srclib/sys_stat.in.h
                # ${LIB_HEADER_DIR}/srclib/sys_types.in.h
                # ${LIB_HEADER_DIR}/srclib/time.h
                # ${LIB_HEADER_DIR}/srclib/time.in.h
                # ${LIB_HEADER_DIR}/srclib/unictype
                # ${LIB_HEADER_DIR}/srclib/unictype/bitmap.h
                # ${LIB_HEADER_DIR}/srclib/unistd.h
                # ${LIB_HEADER_DIR}/srclib/unistd.in.h
                # ${LIB_HEADER_DIR}/srclib/unitypes.h
                # ${LIB_HEADER_DIR}/srclib/unitypes.in.h
                # ${LIB_HEADER_DIR}/srclib/uniwidth
                # ${LIB_HEADER_DIR}/srclib/uniwidth/cjk.h
                # ${LIB_HEADER_DIR}/srclib/uniwidth/width0.h
                # ${LIB_HEADER_DIR}/srclib/uniwidth/width2.h
                # ${LIB_HEADER_DIR}/srclib/uniwidth.h
                # ${LIB_HEADER_DIR}/srclib/uniwidth.in.h
                # ${LIB_HEADER_DIR}/srclib/unlocked-io.h
                # ${LIB_HEADER_DIR}/srclib/verify.h
                # ${LIB_HEADER_DIR}/srclib/warn-on-use.h
                # ${LIB_HEADER_DIR}/srclib/wchar.h
                # ${LIB_HEADER_DIR}/srclib/wchar.in.h
                # ${LIB_HEADER_DIR}/srclib/xalloc-oversized.h
                # ${LIB_HEADER_DIR}/srclib/xalloc.h
                # ${LIB_HEADER_DIR}/srclib/xreadlink.h
                # ${LIB_HEADER_DIR}/srclib/_Noreturn.h
	)
	
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
	set(LIB_SOURCES
                ${LIB_SOURCE_DIR}/srclib/binary-io.c
                ${LIB_SOURCE_DIR}/srclib/relocatable.c
                ${LIB_SOURCE_DIR}/srclib/safe-read.c

  #               ${LIB_SOURCE_DIR}/srclib/allocator.c
                # ${LIB_SOURCE_DIR}/srclib/areadlink.c
                # ${LIB_SOURCE_DIR}/srclib/basename-lgpl.c
                # ${LIB_SOURCE_DIR}/srclib/c-ctype.c
                # ${LIB_SOURCE_DIR}/srclib/canonicalize-lgpl.c
                # ${LIB_SOURCE_DIR}/srclib/careadlinkat.c
                # ${LIB_SOURCE_DIR}/srclib/cloexec.c
                # ${LIB_SOURCE_DIR}/srclib/close.c
                # ${LIB_SOURCE_DIR}/srclib/dup2.c
                # ${LIB_SOURCE_DIR}/srclib/error.c
                # ${LIB_SOURCE_DIR}/srclib/fcntl.c
                # ${LIB_SOURCE_DIR}/srclib/fd-hook.c
                # ${LIB_SOURCE_DIR}/srclib/free.c
                # ${LIB_SOURCE_DIR}/srclib/fstat.c
                # ${LIB_SOURCE_DIR}/srclib/getdtablesize.c
                # ${LIB_SOURCE_DIR}/srclib/getprogname.c
                # ${LIB_SOURCE_DIR}/srclib/malloc/scratch_buffer_grow.c
                # ${LIB_SOURCE_DIR}/srclib/malloc/scratch_buffer_grow_preserve.c
                # ${LIB_SOURCE_DIR}/srclib/malloc/scratch_buffer_set_array_size.c
                # ${LIB_SOURCE_DIR}/srclib/malloc.c
                # ${LIB_SOURCE_DIR}/srclib/malloca.c
                # ${LIB_SOURCE_DIR}/srclib/mempcpy.c
                # # ${LIB_SOURCE_DIR}/srclib/msvc-inval.c
                # # ${LIB_SOURCE_DIR}/srclib/msvc-nothrow.c
                # ${LIB_SOURCE_DIR}/srclib/open.c
                # ${LIB_SOURCE_DIR}/srclib/progname.c
                # ${LIB_SOURCE_DIR}/srclib/progreloc.c
                # ${LIB_SOURCE_DIR}/srclib/raise.c
                # ${LIB_SOURCE_DIR}/srclib/rawmemchr.c
                # ${LIB_SOURCE_DIR}/srclib/read.c
                # ${LIB_SOURCE_DIR}/srclib/readlink.c
                # ${LIB_SOURCE_DIR}/srclib/realloc.c
  #               # ${LIB_SOURCE_DIR}/srclib/relocwrapper.c # bad LIBDIRS
                # ${LIB_SOURCE_DIR}/srclib/setenv.c
                # ${LIB_SOURCE_DIR}/srclib/sigprocmask.c
                # ${LIB_SOURCE_DIR}/srclib/stat-time.c
                # ${LIB_SOURCE_DIR}/srclib/stat-w32.c
                # ${LIB_SOURCE_DIR}/srclib/stat.c
                # ${LIB_SOURCE_DIR}/srclib/stdio-read.c
                # ${LIB_SOURCE_DIR}/srclib/stdio-write.c
                # ${LIB_SOURCE_DIR}/srclib/stdlib.c
                # ${LIB_SOURCE_DIR}/srclib/strerror-override.c
                # ${LIB_SOURCE_DIR}/srclib/strerror.c
                # ${LIB_SOURCE_DIR}/srclib/unistd.c
                # ${LIB_SOURCE_DIR}/srclib/uniwidth
                # ${LIB_SOURCE_DIR}/srclib/uniwidth/width.c
                # ${LIB_SOURCE_DIR}/srclib/xmalloc.c
                # ${LIB_SOURCE_DIR}/srclib/xreadlink.c
                # ${LIB_SOURCE_DIR}/srclib/xstrdup.c
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
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/srclib)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/srclib)
        target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/srclib/malloc)
        target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/srclib/sys)
        target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/srclib/unictype)
        target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/srclib/uniwidth)

	# target_link_libraries(${LIB_NAME} PUBLIC m)

endif()
