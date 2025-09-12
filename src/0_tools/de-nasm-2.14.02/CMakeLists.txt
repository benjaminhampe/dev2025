set(APP_NAME nasm) # We dont like to clash with system libs.
project(${APP_NAME}-2.14.02)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/asm/assemble.c
        ${APP_SOURCE_DIR}/asm/assemble.h
        ${APP_SOURCE_DIR}/asm/directbl.c
        ${APP_SOURCE_DIR}/asm/directiv.c
        # ${APP_SOURCE_DIR}/asm/directiv.dat
        ${APP_SOURCE_DIR}/asm/directiv.h
        ${APP_SOURCE_DIR}/asm/error.c
        ${APP_SOURCE_DIR}/asm/eval.c
        ${APP_SOURCE_DIR}/asm/eval.h
        ${APP_SOURCE_DIR}/asm/exprdump.c
        ${APP_SOURCE_DIR}/asm/exprlib.c
        ${APP_SOURCE_DIR}/asm/float.c
        ${APP_SOURCE_DIR}/asm/float.h
        ${APP_SOURCE_DIR}/asm/labels.c
        ${APP_SOURCE_DIR}/asm/listing.c
        ${APP_SOURCE_DIR}/asm/listing.h
        ${APP_SOURCE_DIR}/asm/nasm.c # main app
        ${APP_SOURCE_DIR}/asm/parser.c
        ${APP_SOURCE_DIR}/asm/parser.h
        # ${APP_SOURCE_DIR}/asm/phash.pl
        ${APP_SOURCE_DIR}/asm/pptok.c
        ${APP_SOURCE_DIR}/asm/pptok.dat
        ${APP_SOURCE_DIR}/asm/pptok.h
        # ${APP_SOURCE_DIR}/asm/pptok.ph
        # ${APP_SOURCE_DIR}/asm/pptok.pl
        ${APP_SOURCE_DIR}/asm/pragma.c
        ${APP_SOURCE_DIR}/asm/preproc.c
        ${APP_SOURCE_DIR}/asm/preproc.h
        ${APP_SOURCE_DIR}/asm/preproc-nop.c
        ${APP_SOURCE_DIR}/asm/quote.c
        ${APP_SOURCE_DIR}/asm/quote.h
        ${APP_SOURCE_DIR}/asm/rdstrnum.c
        ${APP_SOURCE_DIR}/asm/segalloc.c
        ${APP_SOURCE_DIR}/asm/stdscan.c
        ${APP_SOURCE_DIR}/asm/stdscan.h
        ${APP_SOURCE_DIR}/asm/strfunc.c
        # ${APP_SOURCE_DIR}/asm/tokens.dat
        ${APP_SOURCE_DIR}/asm/tokens.h
        ${APP_SOURCE_DIR}/asm/tokhash.c
        # ${APP_SOURCE_DIR}/asm/tokhash.pl

        ${APP_SOURCE_DIR}/common/common.c

        ${APP_SOURCE_DIR}/config/config.h

        ${APP_SOURCE_DIR}/disasm/disasm.c
        ${APP_SOURCE_DIR}/disasm/disasm.h
        # ${APP_SOURCE_DIR}/disasm/ndisasm.c # app
        ${APP_SOURCE_DIR}/disasm/sync.c
        ${APP_SOURCE_DIR}/disasm/sync.h

        ${APP_SOURCE_DIR}/include/bytesex.h
        ${APP_SOURCE_DIR}/include/compiler.h
        ${APP_SOURCE_DIR}/include/disp8.h
        ${APP_SOURCE_DIR}/include/error.h
        ${APP_SOURCE_DIR}/include/hashtbl.h
        ${APP_SOURCE_DIR}/include/iflag.h
        ${APP_SOURCE_DIR}/include/ilog2.h
        ${APP_SOURCE_DIR}/include/insns.h
        ${APP_SOURCE_DIR}/include/labels.h
        ${APP_SOURCE_DIR}/include/md5.h
        ${APP_SOURCE_DIR}/include/nasm.h
        ${APP_SOURCE_DIR}/include/nasmint.h
        ${APP_SOURCE_DIR}/include/nasmlib.h
        ${APP_SOURCE_DIR}/include/opflags.h
        ${APP_SOURCE_DIR}/include/perfhash.h
        ${APP_SOURCE_DIR}/include/raa.h
        ${APP_SOURCE_DIR}/include/rbtree.h
        ${APP_SOURCE_DIR}/include/rdoff.h
        ${APP_SOURCE_DIR}/include/saa.h
        ${APP_SOURCE_DIR}/include/strlist.h
        ${APP_SOURCE_DIR}/include/tables.h
        ${APP_SOURCE_DIR}/include/ver.h

        ${APP_SOURCE_DIR}/macros/macros.c

        # ${APP_SOURCE_DIR}/misc/c16.mac
        # ${APP_SOURCE_DIR}/misc/c32.mac
        # ${APP_SOURCE_DIR}/misc/crcgen.c # app
        # ${APP_SOURCE_DIR}/misc/Doxyfile
        # ${APP_SOURCE_DIR}/misc/exebin2.mac
        # ${APP_SOURCE_DIR}/misc/exebin.mac
        # ${APP_SOURCE_DIR}/misc/fmtinsns.pl
        # ${APP_SOURCE_DIR}/misc/genfma.pl
        # ${APP_SOURCE_DIR}/misc/hints.txt
        # ${APP_SOURCE_DIR}/misc/magic
        # ${APP_SOURCE_DIR}/misc/myC32.mac
        # ${APP_SOURCE_DIR}/misc/nasm.sl
        # ${APP_SOURCE_DIR}/misc/nasmstab
        # ${APP_SOURCE_DIR}/misc/Nindent
        # ${APP_SOURCE_DIR}/misc/omfdump.c # app
        # ${APP_SOURCE_DIR}/misc/pmw.bat
        # ${APP_SOURCE_DIR}/misc/proc32.ash
        # ${APP_SOURCE_DIR}/misc/README
        # ${APP_SOURCE_DIR}/misc/scitech.mac
        # ${APP_SOURCE_DIR}/misc/xcrcgen.c # app

        ${APP_SOURCE_DIR}/nasmlib/badenum.c
        ${APP_SOURCE_DIR}/nasmlib/bsi.c
        ${APP_SOURCE_DIR}/nasmlib/crc64.c
        # ${APP_SOURCE_DIR}/nasmlib/errfile.c # already defined in nasm.c app
        ${APP_SOURCE_DIR}/nasmlib/file.c
        ${APP_SOURCE_DIR}/nasmlib/file.h
        ${APP_SOURCE_DIR}/nasmlib/filename.c
        ${APP_SOURCE_DIR}/nasmlib/hashtbl.c
        ${APP_SOURCE_DIR}/nasmlib/ilog2.c
        ${APP_SOURCE_DIR}/nasmlib/malloc.c
        ${APP_SOURCE_DIR}/nasmlib/md5c.c
        ${APP_SOURCE_DIR}/nasmlib/mmap.c
        ${APP_SOURCE_DIR}/nasmlib/path.c
        ${APP_SOURCE_DIR}/nasmlib/perfhash.c
        ${APP_SOURCE_DIR}/nasmlib/perfhash.pl
        ${APP_SOURCE_DIR}/nasmlib/raa.c
        ${APP_SOURCE_DIR}/nasmlib/rbtree.c
        ${APP_SOURCE_DIR}/nasmlib/readnum.c
        ${APP_SOURCE_DIR}/nasmlib/realpath.c
        ${APP_SOURCE_DIR}/nasmlib/saa.c
        ${APP_SOURCE_DIR}/nasmlib/srcfile.c
        ${APP_SOURCE_DIR}/nasmlib/string.c
        ${APP_SOURCE_DIR}/nasmlib/strlist.c
        ${APP_SOURCE_DIR}/nasmlib/ver.c
        ${APP_SOURCE_DIR}/nasmlib/zerobuf.c

        ${APP_SOURCE_DIR}/output/codeview.c
        ${APP_SOURCE_DIR}/output/dwarf.h
        ${APP_SOURCE_DIR}/output/elf.h
        ${APP_SOURCE_DIR}/output/legacy.c
        ${APP_SOURCE_DIR}/output/nulldbg.c
        ${APP_SOURCE_DIR}/output/nullout.c
        ${APP_SOURCE_DIR}/output/outaout.c
        # ${APP_SOURCE_DIR}/output/outaout.mac
        ${APP_SOURCE_DIR}/output/outas86.c
        # ${APP_SOURCE_DIR}/output/outas86.mac
        ${APP_SOURCE_DIR}/output/outbin.c
        # ${APP_SOURCE_DIR}/output/outbin.mac
        ${APP_SOURCE_DIR}/output/outcoff.c
        # ${APP_SOURCE_DIR}/output/outcoff.mac
        ${APP_SOURCE_DIR}/output/outdbg.c
        # ${APP_SOURCE_DIR}/output/outdbg.mac
        ${APP_SOURCE_DIR}/output/outelf.c
        ${APP_SOURCE_DIR}/output/outelf.h
        # ${APP_SOURCE_DIR}/output/outelf.mac
        ${APP_SOURCE_DIR}/output/outform.c
        ${APP_SOURCE_DIR}/output/outform.h
        ${APP_SOURCE_DIR}/output/outieee.c
        ${APP_SOURCE_DIR}/output/outlib.c
        ${APP_SOURCE_DIR}/output/outlib.h
        ${APP_SOURCE_DIR}/output/outmacho.c
        # ${APP_SOURCE_DIR}/output/outmacho.mac
        ${APP_SOURCE_DIR}/output/outobj.c
        # ${APP_SOURCE_DIR}/output/outobj.mac
        ${APP_SOURCE_DIR}/output/outrdf2.c
        # ${APP_SOURCE_DIR}/output/outrdf2.mac
        # ${APP_SOURCE_DIR}/output/outrdf.mac
        ${APP_SOURCE_DIR}/output/pecoff.h
        ${APP_SOURCE_DIR}/output/stabs.h
        ${APP_SOURCE_DIR}/output/strtbl.c
        ${APP_SOURCE_DIR}/output/strtbl.h

        ${APP_SOURCE_DIR}/rdoff/collectn.c
        ${APP_SOURCE_DIR}/rdoff/collectn.h
        ${APP_SOURCE_DIR}/rdoff/hash.c
        ${APP_SOURCE_DIR}/rdoff/hash.h
        # ${APP_SOURCE_DIR}/rdoff/ldrdf.1
        # ${APP_SOURCE_DIR}/rdoff/ldrdf.c # app
        ${APP_SOURCE_DIR}/rdoff/ldsegs.h
        # ${APP_SOURCE_DIR}/rdoff/rdf2bin.1
        # ${APP_SOURCE_DIR}/rdoff/rdf2bin.c # app
        # ${APP_SOURCE_DIR}/rdoff/rdf2com.1
        # ${APP_SOURCE_DIR}/rdoff/rdf2ihx.1
        # ${APP_SOURCE_DIR}/rdoff/rdf2ith.1
        # ${APP_SOURCE_DIR}/rdoff/rdf2srec.1
        # ${APP_SOURCE_DIR}/rdoff/rdfdump.1
        # ${APP_SOURCE_DIR}/rdoff/rdfdump.c # app
        # ${APP_SOURCE_DIR}/rdoff/rdflib.1
        # ${APP_SOURCE_DIR}/rdoff/rdflib.c  # app
        ${APP_SOURCE_DIR}/rdoff/rdfload.c
        ${APP_SOURCE_DIR}/rdoff/rdfload.h
        ${APP_SOURCE_DIR}/rdoff/rdfutils.h
        #${APP_SOURCE_DIR}/rdoff/rdlar.c
        #${APP_SOURCE_DIR}/rdoff/rdlar.h
        ${APP_SOURCE_DIR}/rdoff/rdlib.c
        ${APP_SOURCE_DIR}/rdoff/rdlib.h
        ${APP_SOURCE_DIR}/rdoff/rdoff.c
        # ${APP_SOURCE_DIR}/rdoff/rdx.1
        # ${APP_SOURCE_DIR}/rdoff/rdx.c # app
        # ${APP_SOURCE_DIR}/rdoff/README
        ${APP_SOURCE_DIR}/rdoff/segtab.c
        ${APP_SOURCE_DIR}/rdoff/segtab.h
        ${APP_SOURCE_DIR}/rdoff/symtab.c
        ${APP_SOURCE_DIR}/rdoff/symtab.h

        ${APP_SOURCE_DIR}/stdlib/snprintf.c
        ${APP_SOURCE_DIR}/stdlib/strlcpy.c
        ${APP_SOURCE_DIR}/stdlib/strnlen.c
        ${APP_SOURCE_DIR}/stdlib/strrchrnul.c
        ${APP_SOURCE_DIR}/stdlib/vsnprintf.c

        ${APP_SOURCE_DIR}/x86/disp8.c
        ${APP_SOURCE_DIR}/x86/iflag.c
        ${APP_SOURCE_DIR}/x86/iflaggen.h
        ${APP_SOURCE_DIR}/x86/insnsa.c
        ${APP_SOURCE_DIR}/x86/insnsb.c
        # ${APP_SOURCE_DIR}/x86/insns.dat
        ${APP_SOURCE_DIR}/x86/insnsd.c
        # ${APP_SOURCE_DIR}/x86/insns-iflags.ph
        ${APP_SOURCE_DIR}/x86/insnsi.h
        ${APP_SOURCE_DIR}/x86/insnsn.c
        # ${APP_SOURCE_DIR}/x86/insns.pl
        ${APP_SOURCE_DIR}/x86/regdis.c
        ${APP_SOURCE_DIR}/x86/regdis.h
        ${APP_SOURCE_DIR}/x86/regflags.c
        ${APP_SOURCE_DIR}/x86/regs.c
        # ${APP_SOURCE_DIR}/x86/regs.dat
        ${APP_SOURCE_DIR}/x86/regs.h
        # ${APP_SOURCE_DIR}/x86/regs.pl
        ${APP_SOURCE_DIR}/x86/regvals.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    #    target_compile_definitions(${APP_NAME} PUBLIC -DNASM_BUILD_LIBRARY)

    target_compile_definitions(${APP_NAME} PUBLIC -DNASM_MAJOR_VER=2)
    target_compile_definitions(${APP_NAME} PUBLIC -DNASM_MINOR_VER=14)
    target_compile_definitions(${APP_NAME} PUBLIC -DNASM_SUBMINOR_VER=2)
    target_compile_definitions(${APP_NAME} PUBLIC -DNASM_PATCHLEVEL_VER=0)
    target_compile_definitions(${APP_NAME} PUBLIC -DNASM_VERSION_ID=0x020e0200)
    target_compile_definitions(${APP_NAME} PUBLIC -DNASM_VER="2.14.02")
    target_compile_definitions(${APP_NAME} PRIVATE -DHAVE_CONFIG_H=1)

    target_sources(${APP_NAME} PUBLIC ${NASM_HEADERS} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/include)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/asm)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/disasm)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/config)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/output)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/x86)

    target_compile_options(${APP_NAME} PRIVATE
                            -O3
                            -fwrapv
                            -U__STRICT_ANSI__
                            -fno-common
                            -Werror=attributes
                            -fvisibility=hidden
                            -W
                            -Wall
                            -pedantic
                            -Wc99-compat # -Wc90-c99-compat
                            -Wno-long-long
                            -Wno-shift-negative-value
                            -Werror=implicit
                            -Werror=missing-braces
                            -Werror=return-type
                            -Werror=trigraphs
                            -Werror=pointer-arith
                            -Werror=missing-prototypes
                            -Werror=missing-declarations
                            #-Wno-error=comment
                            -Werror=vla
                            -Wno-pedantic
    )

    ### add_custom_command(TARGET foo POST_BUILD
    ###    COMMAND ${CMAKE_COMMAND} -E copy
    ###            ${CMAKE_SOURCE_DIR}/test/input.txt
    ###            ${CMAKE_CURRENT_BINARY_DIR}/input.txt)
endif()
