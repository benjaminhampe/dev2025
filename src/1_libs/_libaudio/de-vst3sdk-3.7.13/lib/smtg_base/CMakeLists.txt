set(LIB_NAME smtg_base)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/base/source/classfactoryhelpers.h
        ${LIB_HEADER_DIR}/base/source/fbuffer.h
        ${LIB_HEADER_DIR}/base/source/fcleanup.h
        ${LIB_HEADER_DIR}/base/source/fcommandline.h
        ${LIB_HEADER_DIR}/base/source/fdebug.h
        ${LIB_HEADER_DIR}/base/source/fdynlib.h
        ${LIB_HEADER_DIR}/base/source/fobject.h
        ${LIB_HEADER_DIR}/base/source/fstdmethods.h
        ${LIB_HEADER_DIR}/base/source/fstreamer.h
        ${LIB_HEADER_DIR}/base/source/fstring.h
        ${LIB_HEADER_DIR}/base/source/hexbinary.h
        ${LIB_HEADER_DIR}/base/source/timer.h
        ${LIB_HEADER_DIR}/base/source/updatehandler.h
        ${LIB_HEADER_DIR}/base/thread/include/fcondition.h
        ${LIB_HEADER_DIR}/base/thread/include/flock.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/conststringtable.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/doc.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/falignpop.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/falignpush.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/fplatform.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/fstrdefs.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/ftypes.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/funknown.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/funknownimpl.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/futils.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/fvariant.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/geoconstants.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/ibstream.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/icloneable.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/ierrorcontext.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/ipersistent.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/ipluginbase.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/iplugincompatibility.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/istringresult.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/iupdatehandler.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/keycodes.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/pluginbasefwd.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/smartpointer.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/typesizecheck.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/ucolorspec.h
        ${LIB_HEADER_DIR}/pluginterfaces/base/ustring.h
        # ${LIB_HEADER_DIR}/gui
        ${LIB_HEADER_DIR}/pluginterfaces/gui/iplugview.h
        ${LIB_HEADER_DIR}/pluginterfaces/gui/iplugviewcontentscalesupport.h
        # ${LIB_HEADER_DIR}/pluginterfaces/test
        ${LIB_HEADER_DIR}/pluginterfaces/test/itest.h
        # ${LIB_HEADER_DIR}/pluginterfaces/vst
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstattributes.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstaudioprocessor.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstautomationstate.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstchannelcontextinfo.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstcomponent.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstcontextmenu.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstdataexchange.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivsteditcontroller.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstevents.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivsthostapplication.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstinterappaudio.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstmessage.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstmidicontrollers.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstmidilearn.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstnoteexpression.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstparameterchanges.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstparameterfunctionname.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstphysicalui.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstpluginterfacesupport.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstplugview.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstprefetchablesupport.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstprocesscontext.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstremapparamid.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstrepresentation.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivsttestplugprovider.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstunits.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/vstpresetkeys.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/vstpshpack4.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/vstspeaker.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst/vsttypes.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/base/source/baseiids.cpp
        ${LIB_SOURCE_DIR}/base/source/fbuffer.cpp
        ${LIB_SOURCE_DIR}/base/source/fdebug.cpp
        ${LIB_SOURCE_DIR}/base/source/fdynlib.cpp
        ${LIB_SOURCE_DIR}/base/source/fobject.cpp
        ${LIB_SOURCE_DIR}/base/source/fstreamer.cpp
        ${LIB_SOURCE_DIR}/base/source/fstring.cpp
        ${LIB_SOURCE_DIR}/base/source/timer.cpp
        ${LIB_SOURCE_DIR}/base/source/updatehandler.cpp
        ${LIB_SOURCE_DIR}/base/thread/source/fcondition.cpp
        ${LIB_SOURCE_DIR}/base/thread/source/flock.cpp
        ${LIB_SOURCE_DIR}/pluginterfaces/base/conststringtable.cpp
        ${LIB_SOURCE_DIR}/pluginterfaces/base/coreiids.cpp
        ${LIB_SOURCE_DIR}/pluginterfaces/base/funknown.cpp
        ${LIB_SOURCE_DIR}/pluginterfaces/base/ustring.cpp		
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/base/source)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/pluginterfaces/base)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	# "$<$<CONFIG:Debug>:-DDEVELOPMENT=1>"
	# "$<$<CONFIG:Release>:-DRELEASE=1>"
	# "$<$<CONFIG:RelWithDebInfo>:-DRELEASE=1>"
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_definitions(${LIB_NAME} PRIVATE DEVELOPMENT=1)
	else()
		target_compile_definitions(${LIB_NAME} PRIVATE RELEASE=1)        
	endif()

    if (MSVC)
        # set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /IGNORE:4221")
    else()
        # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-pragma-pack)
    endif()
endif()


