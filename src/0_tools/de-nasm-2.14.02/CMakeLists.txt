set(DE_NASM nasm) # We dont like to clash with system libs.
project(de-${DE_NASM}-2.14.02-dev)

if (TARGET ${DE_NASM})
   message("${BoldYellow}[benni] HEADER_GUARD: ${DE_NASM}${ColourReset}")
else()

   if (WIN32)
      enable_language(RC)
   endif()
#    set(NASM_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
   set(NASM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

#    set(NASM_HEADERS
#      # Config headers
#       ${NASM_HEADER_DIR}/ft2build.h
#      # Internal headers
#       ${NASM_HEADER_DIR}/freetype/internal/tttypes.h
#    )

   set(NASM_SOURCES
      ${NASM_SOURCE_DIR}/asm/assemble.c
      ${NASM_SOURCE_DIR}/asm/assemble.h
      ${NASM_SOURCE_DIR}/asm/directbl.c
      ${NASM_SOURCE_DIR}/asm/directiv.c
      # ${NASM_SOURCE_DIR}/asm/directiv.dat
      ${NASM_SOURCE_DIR}/asm/directiv.h
      ${NASM_SOURCE_DIR}/asm/error.c
      ${NASM_SOURCE_DIR}/asm/eval.c
      ${NASM_SOURCE_DIR}/asm/eval.h
      ${NASM_SOURCE_DIR}/asm/exprdump.c
      ${NASM_SOURCE_DIR}/asm/exprlib.c
      ${NASM_SOURCE_DIR}/asm/float.c
      ${NASM_SOURCE_DIR}/asm/float.h
      ${NASM_SOURCE_DIR}/asm/labels.c
      ${NASM_SOURCE_DIR}/asm/listing.c
      ${NASM_SOURCE_DIR}/asm/listing.h
      ${NASM_SOURCE_DIR}/asm/nasm.c # main app
      ${NASM_SOURCE_DIR}/asm/parser.c
      ${NASM_SOURCE_DIR}/asm/parser.h
      # ${NASM_SOURCE_DIR}/asm/phash.pl
      ${NASM_SOURCE_DIR}/asm/pptok.c
      ${NASM_SOURCE_DIR}/asm/pptok.dat
      ${NASM_SOURCE_DIR}/asm/pptok.h
      # ${NASM_SOURCE_DIR}/asm/pptok.ph
      # ${NASM_SOURCE_DIR}/asm/pptok.pl
      ${NASM_SOURCE_DIR}/asm/pragma.c
      ${NASM_SOURCE_DIR}/asm/preproc.c
      ${NASM_SOURCE_DIR}/asm/preproc.h
      ${NASM_SOURCE_DIR}/asm/preproc-nop.c
      ${NASM_SOURCE_DIR}/asm/quote.c
      ${NASM_SOURCE_DIR}/asm/quote.h
      ${NASM_SOURCE_DIR}/asm/rdstrnum.c
      ${NASM_SOURCE_DIR}/asm/segalloc.c
      ${NASM_SOURCE_DIR}/asm/stdscan.c
      ${NASM_SOURCE_DIR}/asm/stdscan.h
      ${NASM_SOURCE_DIR}/asm/strfunc.c
      # ${NASM_SOURCE_DIR}/asm/tokens.dat
      ${NASM_SOURCE_DIR}/asm/tokens.h
      ${NASM_SOURCE_DIR}/asm/tokhash.c
      # ${NASM_SOURCE_DIR}/asm/tokhash.pl

      ${NASM_SOURCE_DIR}/common/common.c

      ${NASM_SOURCE_DIR}/config/config.h

      ${NASM_SOURCE_DIR}/disasm/disasm.c
      ${NASM_SOURCE_DIR}/disasm/disasm.h
      # ${NASM_SOURCE_DIR}/disasm/ndisasm.c # app
      ${NASM_SOURCE_DIR}/disasm/sync.c
      ${NASM_SOURCE_DIR}/disasm/sync.h

      ${NASM_SOURCE_DIR}/include/bytesex.h
      ${NASM_SOURCE_DIR}/include/compiler.h
      ${NASM_SOURCE_DIR}/include/disp8.h
      ${NASM_SOURCE_DIR}/include/error.h
      ${NASM_SOURCE_DIR}/include/hashtbl.h
      ${NASM_SOURCE_DIR}/include/iflag.h
      ${NASM_SOURCE_DIR}/include/ilog2.h
      ${NASM_SOURCE_DIR}/include/insns.h
      ${NASM_SOURCE_DIR}/include/labels.h
      ${NASM_SOURCE_DIR}/include/md5.h
      ${NASM_SOURCE_DIR}/include/nasm.h
      ${NASM_SOURCE_DIR}/include/nasmint.h
      ${NASM_SOURCE_DIR}/include/nasmlib.h
      ${NASM_SOURCE_DIR}/include/opflags.h
      ${NASM_SOURCE_DIR}/include/perfhash.h
      ${NASM_SOURCE_DIR}/include/raa.h
      ${NASM_SOURCE_DIR}/include/rbtree.h
      ${NASM_SOURCE_DIR}/include/rdoff.h
      ${NASM_SOURCE_DIR}/include/saa.h
      ${NASM_SOURCE_DIR}/include/strlist.h
      ${NASM_SOURCE_DIR}/include/tables.h
      ${NASM_SOURCE_DIR}/include/ver.h

      ${NASM_SOURCE_DIR}/macros/macros.c

      # ${NASM_SOURCE_DIR}/misc/c16.mac
      # ${NASM_SOURCE_DIR}/misc/c32.mac
      # ${NASM_SOURCE_DIR}/misc/crcgen.c # app
      # ${NASM_SOURCE_DIR}/misc/Doxyfile
      # ${NASM_SOURCE_DIR}/misc/exebin2.mac
      # ${NASM_SOURCE_DIR}/misc/exebin.mac
      # ${NASM_SOURCE_DIR}/misc/fmtinsns.pl
      # ${NASM_SOURCE_DIR}/misc/genfma.pl
      # ${NASM_SOURCE_DIR}/misc/hints.txt
      # ${NASM_SOURCE_DIR}/misc/magic
      # ${NASM_SOURCE_DIR}/misc/myC32.mac
      # ${NASM_SOURCE_DIR}/misc/nasm.sl
      # ${NASM_SOURCE_DIR}/misc/nasmstab
      # ${NASM_SOURCE_DIR}/misc/Nindent
      # ${NASM_SOURCE_DIR}/misc/omfdump.c # app
      # ${NASM_SOURCE_DIR}/misc/pmw.bat
      # ${NASM_SOURCE_DIR}/misc/proc32.ash
      # ${NASM_SOURCE_DIR}/misc/README
      # ${NASM_SOURCE_DIR}/misc/scitech.mac
      # ${NASM_SOURCE_DIR}/misc/xcrcgen.c # app

      ${NASM_SOURCE_DIR}/nasmlib/badenum.c
      ${NASM_SOURCE_DIR}/nasmlib/bsi.c
      ${NASM_SOURCE_DIR}/nasmlib/crc64.c
      # ${NASM_SOURCE_DIR}/nasmlib/errfile.c # already defined in nasm.c app
      ${NASM_SOURCE_DIR}/nasmlib/file.c
      ${NASM_SOURCE_DIR}/nasmlib/file.h
      ${NASM_SOURCE_DIR}/nasmlib/filename.c
      ${NASM_SOURCE_DIR}/nasmlib/hashtbl.c
      ${NASM_SOURCE_DIR}/nasmlib/ilog2.c
      ${NASM_SOURCE_DIR}/nasmlib/malloc.c
      ${NASM_SOURCE_DIR}/nasmlib/md5c.c
      ${NASM_SOURCE_DIR}/nasmlib/mmap.c
      ${NASM_SOURCE_DIR}/nasmlib/path.c
      ${NASM_SOURCE_DIR}/nasmlib/perfhash.c
      ${NASM_SOURCE_DIR}/nasmlib/perfhash.pl
      ${NASM_SOURCE_DIR}/nasmlib/raa.c
      ${NASM_SOURCE_DIR}/nasmlib/rbtree.c
      ${NASM_SOURCE_DIR}/nasmlib/readnum.c
      ${NASM_SOURCE_DIR}/nasmlib/realpath.c
      ${NASM_SOURCE_DIR}/nasmlib/saa.c
      ${NASM_SOURCE_DIR}/nasmlib/srcfile.c
      ${NASM_SOURCE_DIR}/nasmlib/string.c
      ${NASM_SOURCE_DIR}/nasmlib/strlist.c
      ${NASM_SOURCE_DIR}/nasmlib/ver.c
      ${NASM_SOURCE_DIR}/nasmlib/zerobuf.c

      ${NASM_SOURCE_DIR}/output/codeview.c
      ${NASM_SOURCE_DIR}/output/dwarf.h
      ${NASM_SOURCE_DIR}/output/elf.h
      ${NASM_SOURCE_DIR}/output/legacy.c
      ${NASM_SOURCE_DIR}/output/nulldbg.c
      ${NASM_SOURCE_DIR}/output/nullout.c
      ${NASM_SOURCE_DIR}/output/outaout.c
      # ${NASM_SOURCE_DIR}/output/outaout.mac
      ${NASM_SOURCE_DIR}/output/outas86.c
      # ${NASM_SOURCE_DIR}/output/outas86.mac
      ${NASM_SOURCE_DIR}/output/outbin.c
      # ${NASM_SOURCE_DIR}/output/outbin.mac
      ${NASM_SOURCE_DIR}/output/outcoff.c
      # ${NASM_SOURCE_DIR}/output/outcoff.mac
      ${NASM_SOURCE_DIR}/output/outdbg.c
      # ${NASM_SOURCE_DIR}/output/outdbg.mac
      ${NASM_SOURCE_DIR}/output/outelf.c
      ${NASM_SOURCE_DIR}/output/outelf.h
      # ${NASM_SOURCE_DIR}/output/outelf.mac
      ${NASM_SOURCE_DIR}/output/outform.c
      ${NASM_SOURCE_DIR}/output/outform.h
      ${NASM_SOURCE_DIR}/output/outieee.c
      ${NASM_SOURCE_DIR}/output/outlib.c
      ${NASM_SOURCE_DIR}/output/outlib.h
      ${NASM_SOURCE_DIR}/output/outmacho.c
      # ${NASM_SOURCE_DIR}/output/outmacho.mac
      ${NASM_SOURCE_DIR}/output/outobj.c
      # ${NASM_SOURCE_DIR}/output/outobj.mac
      ${NASM_SOURCE_DIR}/output/outrdf2.c
      # ${NASM_SOURCE_DIR}/output/outrdf2.mac
      # ${NASM_SOURCE_DIR}/output/outrdf.mac
      ${NASM_SOURCE_DIR}/output/pecoff.h
      ${NASM_SOURCE_DIR}/output/stabs.h
      ${NASM_SOURCE_DIR}/output/strtbl.c
      ${NASM_SOURCE_DIR}/output/strtbl.h

      ${NASM_SOURCE_DIR}/rdoff/collectn.c
      ${NASM_SOURCE_DIR}/rdoff/collectn.h
      ${NASM_SOURCE_DIR}/rdoff/hash.c
      ${NASM_SOURCE_DIR}/rdoff/hash.h
      # ${NASM_SOURCE_DIR}/rdoff/ldrdf.1
      # ${NASM_SOURCE_DIR}/rdoff/ldrdf.c # app
      ${NASM_SOURCE_DIR}/rdoff/ldsegs.h
      # ${NASM_SOURCE_DIR}/rdoff/rdf2bin.1
      # ${NASM_SOURCE_DIR}/rdoff/rdf2bin.c # app
      # ${NASM_SOURCE_DIR}/rdoff/rdf2com.1
      # ${NASM_SOURCE_DIR}/rdoff/rdf2ihx.1
      # ${NASM_SOURCE_DIR}/rdoff/rdf2ith.1
      # ${NASM_SOURCE_DIR}/rdoff/rdf2srec.1
      # ${NASM_SOURCE_DIR}/rdoff/rdfdump.1
      # ${NASM_SOURCE_DIR}/rdoff/rdfdump.c # app
      # ${NASM_SOURCE_DIR}/rdoff/rdflib.1
      # ${NASM_SOURCE_DIR}/rdoff/rdflib.c  # app
      ${NASM_SOURCE_DIR}/rdoff/rdfload.c
      ${NASM_SOURCE_DIR}/rdoff/rdfload.h
      ${NASM_SOURCE_DIR}/rdoff/rdfutils.h
      #${NASM_SOURCE_DIR}/rdoff/rdlar.c
      #${NASM_SOURCE_DIR}/rdoff/rdlar.h
      ${NASM_SOURCE_DIR}/rdoff/rdlib.c
      ${NASM_SOURCE_DIR}/rdoff/rdlib.h
      ${NASM_SOURCE_DIR}/rdoff/rdoff.c
      # ${NASM_SOURCE_DIR}/rdoff/rdx.1
      # ${NASM_SOURCE_DIR}/rdoff/rdx.c # app
      # ${NASM_SOURCE_DIR}/rdoff/README
      ${NASM_SOURCE_DIR}/rdoff/segtab.c
      ${NASM_SOURCE_DIR}/rdoff/segtab.h
      ${NASM_SOURCE_DIR}/rdoff/symtab.c
      ${NASM_SOURCE_DIR}/rdoff/symtab.h

      ${NASM_SOURCE_DIR}/stdlib/snprintf.c
      ${NASM_SOURCE_DIR}/stdlib/strlcpy.c
      ${NASM_SOURCE_DIR}/stdlib/strnlen.c
      ${NASM_SOURCE_DIR}/stdlib/strrchrnul.c
      ${NASM_SOURCE_DIR}/stdlib/vsnprintf.c

      ${NASM_SOURCE_DIR}/x86/disp8.c
      ${NASM_SOURCE_DIR}/x86/iflag.c
      ${NASM_SOURCE_DIR}/x86/iflaggen.h
      ${NASM_SOURCE_DIR}/x86/insnsa.c
      ${NASM_SOURCE_DIR}/x86/insnsb.c
      # ${NASM_SOURCE_DIR}/x86/insns.dat
      ${NASM_SOURCE_DIR}/x86/insnsd.c
      # ${NASM_SOURCE_DIR}/x86/insns-iflags.ph
      ${NASM_SOURCE_DIR}/x86/insnsi.h
      ${NASM_SOURCE_DIR}/x86/insnsn.c
      # ${NASM_SOURCE_DIR}/x86/insns.pl
      ${NASM_SOURCE_DIR}/x86/regdis.c
      ${NASM_SOURCE_DIR}/x86/regdis.h
      ${NASM_SOURCE_DIR}/x86/regflags.c
      ${NASM_SOURCE_DIR}/x86/regs.c
      # ${NASM_SOURCE_DIR}/x86/regs.dat
      ${NASM_SOURCE_DIR}/x86/regs.h
      # ${NASM_SOURCE_DIR}/x86/regs.pl
      ${NASM_SOURCE_DIR}/x86/regvals.c
   )

   if (WIN32)
      enable_language(RC)
      list(APPEND NASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
      list(APPEND NASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
   endif()

   message("${BoldGreen}[benni] TOOL: ${DE_NASM}${ColourReset}")
   add_executable(${DE_NASM} "")

#    target_compile_definitions(${DE_NASM} PUBLIC -DNASM_BUILD_LIBRARY)

   target_compile_definitions(${DE_NASM} PUBLIC -DNASM_MAJOR_VER=2)
   target_compile_definitions(${DE_NASM} PUBLIC -DNASM_MINOR_VER=14)
   target_compile_definitions(${DE_NASM} PUBLIC -DNASM_SUBMINOR_VER=2)
   target_compile_definitions(${DE_NASM} PUBLIC -DNASM_PATCHLEVEL_VER=0)
   target_compile_definitions(${DE_NASM} PUBLIC -DNASM_VERSION_ID=0x020e0200)
   target_compile_definitions(${DE_NASM} PUBLIC -DNASM_VER="2.14.02")
   target_compile_definitions(${DE_NASM} PRIVATE -DHAVE_CONFIG_H=1)

   target_sources(${DE_NASM} PUBLIC ${NASM_HEADERS} PRIVATE ${NASM_SOURCES})

   target_include_directories(${DE_NASM} PRIVATE ${NASM_SOURCE_DIR})
   target_include_directories(${DE_NASM} PRIVATE ${NASM_SOURCE_DIR}/include)
   target_include_directories(${DE_NASM} PRIVATE ${NASM_SOURCE_DIR}/asm)
   target_include_directories(${DE_NASM} PRIVATE ${NASM_SOURCE_DIR}/disasm)
   target_include_directories(${DE_NASM} PRIVATE ${NASM_SOURCE_DIR}/config)
   target_include_directories(${DE_NASM} PRIVATE ${NASM_SOURCE_DIR}/output)
   target_include_directories(${DE_NASM} PRIVATE ${NASM_SOURCE_DIR}/x86)

   target_compile_options(${DE_NASM} PRIVATE
                                    -O3
                                    -fwrapv
                                    -U__STRICT_ANSI__
                                    -fno-common
                                    -Werror=attributes
                                    -fvisibility=hidden
                                    -W
                                    -Wall
                                    -pedantic
                                    -Wc90-c99-compat
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
