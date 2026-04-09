set(LIB_NAME de_vst3sdk_380)
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

        ${LIB_HEADER_DIR}/pluginterfaces/gui/iplugview.h
        ${LIB_HEADER_DIR}/pluginterfaces/gui/iplugviewcontentscalesupport.h
        ${LIB_HEADER_DIR}/pluginterfaces/gui/iwaylandframe.h

        ${LIB_HEADER_DIR}/pluginterfaces/test/itest.h

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
        ${LIB_HEADER_DIR}/pluginterfaces/vst/ivstmidimapping2.h
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

        ${LIB_HEADER_DIR}/public.sdk/source/common/commonstringconvert.h
        ${LIB_HEADER_DIR}/public.sdk/source/common/memorystream.h
        ${LIB_HEADER_DIR}/public.sdk/source/common/openurl.h
        ${LIB_HEADER_DIR}/public.sdk/source/common/pluginview.h
        ${LIB_HEADER_DIR}/public.sdk/source/common/readfile.h
        ${LIB_HEADER_DIR}/public.sdk/source/common/systemclipboard.h
        ${LIB_HEADER_DIR}/public.sdk/source/common/threadchecker.h

        ${LIB_HEADER_DIR}/public.sdk/source/main/moduleinit.h
        ${LIB_HEADER_DIR}/public.sdk/source/main/pluginfactory.h
        ${LIB_HEADER_DIR}/public.sdk/source/main/pluginfactory_constexpr.h

        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/connectionproxy.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/eventlist.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/hostclasses.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/hostdataexchangehandler.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/module.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/parameterchanges.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/pluginterfacesupport.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/plugprovider.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting/processdata.h

        ${LIB_HEADER_DIR}/public.sdk/source/vst/moduleinfo/json.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/moduleinfo/jsoncxx.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/moduleinfo/moduleinfo.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/moduleinfo/moduleinfocreator.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/moduleinfo/moduleinfoparser.h

        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/alignedalloc.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/audiobuffers.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/dataexchange.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/memoryibstream.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/mpeprocessor.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/objcclassbuilder.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/optional.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/processcontextrequirements.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/processdataslicer.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/ringbuffer.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/rttransfer.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/sampleaccurate.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/stringconvert.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/systemtime.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/testing.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/uid.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/ump.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/versionparser.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/utility/vst2persistence.h
		
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstaudioeffect.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstaudioprocessoralgo.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstbus.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstbypassprocessor.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstcomponent.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstcomponentbase.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vsteditcontroller.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vsteventshelper.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstguieditor.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstgui_linux_runloop_support.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstgui_win32_bundle_support.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vsthelpers.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstnoteexpressiontypes.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstparameters.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstpresetfile.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstrepresentation.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.h
        ${LIB_HEADER_DIR}/public.sdk/source/vst/vstspeakerarray.h
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

        ${LIB_SOURCE_DIR}/public.sdk/source/common/commoniids.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/common/commonstringconvert.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/common/memorystream.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/common/openurl.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/common/pluginview.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/common/readfile.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/common/systemclipboard_linux.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/common/systemclipboard_mac.mm
        ${LIB_SOURCE_DIR}/public.sdk/source/common/systemclipboard_win32.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/common/threadchecker_linux.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/common/threadchecker_mac.mm
        ${LIB_SOURCE_DIR}/public.sdk/source/common/threadchecker_win32.cpp

        # ${LIB_SOURCE_DIR}/public.sdk/source/main/dllmain.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/main/linuxmain.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/main/macexport.exp
        # ${LIB_SOURCE_DIR}/public.sdk/source/main/macmain.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/main/moduleinit.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/main/pluginfactory.cpp

        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/connectionproxy.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/eventlist.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/hostclasses.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/hostdataexchangehandler.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/module.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/module_linux.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/module_mac.mm
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/module_win32.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/parameterchanges.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/pluginterfacesupport.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/plugprovider.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/hosting/processdata.cpp

        ${LIB_SOURCE_DIR}/public.sdk/source/vst/moduleinfo/moduleinfocreator.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/moduleinfo/moduleinfoparser.cpp

        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/dataexchange.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/mpeprocessor.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/stringconvert.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/systemtime.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/testing.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/utility/vst2persistence.cpp

        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstaudioeffect.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstbus.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstcomponent.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstcomponentbase.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vsteditcontroller.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstguieditor.cpp # VSTGUI?
        # ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstgui_linux_runloop_support.cpp # VSTGUI?
        # ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstgui_win32_bundle_support.cpp # VSTGUI?
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstinitiids.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstnoteexpressiontypes.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstparameters.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstpresetfile.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstrepresentation.cpp
        ${LIB_SOURCE_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp
    )
	
	# if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_VST3SDK=1)

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/base/source)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/pluginterfaces/base)

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/common)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/main)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/vst)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/vst/hosting)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/vst/moduleinfo)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/public.sdk/source/vst/utility)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	# "$<$<CONFIG:Debug>:-DDEVELOPMENT=1>"
	# "$<$<CONFIG:Release>:-DRELEASE=1>"
	# "$<$<CONFIG:RelWithDebInfo>:-DRELEASE=1>"
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_definitions(${LIB_NAME} PRIVATE DEVELOPMENT=1)
	else()
		target_compile_definitions(${LIB_NAME} PRIVATE RELEASE=1)
	endif()
	
	# target_link_libraries(${LIB_NAME} PUBLIC smtg_sdk_common)

endif()


