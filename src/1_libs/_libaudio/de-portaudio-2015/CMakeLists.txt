set(LIB_NAME de_portaudio_2015) # We dont like to clash with system libs.
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

	set(LIB_HEADERS
		# ${LIB_HEADER_DIR}/msinttypes/changelog.txt
		${LIB_HEADER_DIR}/msinttypes/inttypes.h
		${LIB_HEADER_DIR}/msinttypes/stdint.h
		${LIB_HEADER_DIR}/pa_asio.h
		${LIB_HEADER_DIR}/pa_jack.h
		${LIB_HEADER_DIR}/pa_linux_alsa.h
		${LIB_HEADER_DIR}/pa_mac_core.h
		${LIB_HEADER_DIR}/pa_win_ds.h
		${LIB_HEADER_DIR}/pa_win_wasapi.h
		${LIB_HEADER_DIR}/pa_win_waveformat.h
		${LIB_HEADER_DIR}/pa_win_wdmks.h
		${LIB_HEADER_DIR}/pa_win_wmme.h
		${LIB_HEADER_DIR}/portaudio.h
	)
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/common/pa_allocation.c
		${LIB_SOURCE_DIR}/common/pa_allocation.h
		${LIB_SOURCE_DIR}/common/pa_converters.c
		${LIB_SOURCE_DIR}/common/pa_converters.h
		${LIB_SOURCE_DIR}/common/pa_cpuload.c
		${LIB_SOURCE_DIR}/common/pa_cpuload.h
		${LIB_SOURCE_DIR}/common/pa_debugprint.c
		${LIB_SOURCE_DIR}/common/pa_debugprint.h
		${LIB_SOURCE_DIR}/common/pa_dither.c
		${LIB_SOURCE_DIR}/common/pa_dither.h
		${LIB_SOURCE_DIR}/common/pa_endianness.h
		${LIB_SOURCE_DIR}/common/pa_front.c
		${LIB_SOURCE_DIR}/common/pa_hostapi.h
		${LIB_SOURCE_DIR}/common/pa_memorybarrier.h
		${LIB_SOURCE_DIR}/common/pa_process.c
		${LIB_SOURCE_DIR}/common/pa_process.h
		${LIB_SOURCE_DIR}/common/pa_ringbuffer.c
		${LIB_SOURCE_DIR}/common/pa_ringbuffer.h
		${LIB_SOURCE_DIR}/common/pa_stream.c
		${LIB_SOURCE_DIR}/common/pa_stream.h
		${LIB_SOURCE_DIR}/common/pa_svnrevision.h
		${LIB_SOURCE_DIR}/common/pa_trace.c
		${LIB_SOURCE_DIR}/common/pa_trace.h
		${LIB_SOURCE_DIR}/common/pa_types.h
		${LIB_SOURCE_DIR}/common/pa_util.h

		# ${LIB_SOURCE_DIR}/hostapi/alsa/pa_linux_alsa.c

		# ${LIB_SOURCE_DIR}/hostapi/asihpi/pa_linux_asihpi.c

		# ${LIB_SOURCE_DIR}/hostapi/asio/ASIO-README.txt
		# ${LIB_SOURCE_DIR}/hostapi/asio/Callback_adaptation_.pdf
		${LIB_SOURCE_DIR}/hostapi/asio/iasiothiscallresolver.cpp
		${LIB_SOURCE_DIR}/hostapi/asio/iasiothiscallresolver.h
		${LIB_SOURCE_DIR}/hostapi/asio/pa_asio.cpp
		# ${LIB_SOURCE_DIR}/hostapi/asio/Pa_ASIO.pdf
		
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/notes.txt
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/pa_mac_core.c
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/pa_mac_core_blocking.c
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/pa_mac_core_blocking.h
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/pa_mac_core_internal.h
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/pa_mac_core_old.c
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/pa_mac_core_utilities.c
		# ${LIB_SOURCE_DIR}/hostapi/coreaudio/pa_mac_core_utilities.h
		
		${LIB_SOURCE_DIR}/hostapi/dsound/pa_win_ds.c
		${LIB_SOURCE_DIR}/hostapi/dsound/pa_win_ds_dynlink.c
		${LIB_SOURCE_DIR}/hostapi/dsound/pa_win_ds_dynlink.h
		
		# ${LIB_SOURCE_DIR}/hostapi/jack/pa_jack.c
		
		# ${LIB_SOURCE_DIR}/hostapi/oss/low_latency_tip.txt
		# ${LIB_SOURCE_DIR}/hostapi/oss/pa_unix_oss.c
		# ${LIB_SOURCE_DIR}/hostapi/oss/recplay.c
		
		${LIB_SOURCE_DIR}/hostapi/skeleton/pa_hostapi_skeleton.c
		
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/audioclient.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/AudioSessionTypes.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/devicetopology.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/endpointvolume.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/functiondiscoverykeys.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/FunctionDiscoveryKeys_devpkey.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/ks.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/ksguid.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/ksmedia.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/ksproxy.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/ksuuids.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/mmdeviceapi.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/propidl.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/propkey.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/propkeydef.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/propsys.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/rpcsal.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/sal.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/sdkddkver.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/shtypes.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/mingw-include/structuredquery.h
		${LIB_SOURCE_DIR}/hostapi/wasapi/pa_win_wasapi.c

        # ${LIB_SOURCE_DIR}/hostapi/wdmks/pa_win_wdmks.c
		# ${LIB_SOURCE_DIR}/hostapi/wdmks/readme.txt
		${LIB_SOURCE_DIR}/hostapi/wmme/pa_win_wmme.c

		# ${LIB_SOURCE_DIR}/os/unix/pa_unix_hostapis.c
		# ${LIB_SOURCE_DIR}/os/unix/pa_unix_util.c
		# ${LIB_SOURCE_DIR}/os/unix/pa_unix_util.h

		${LIB_SOURCE_DIR}/os/win/pa_win_coinitialize.c
		${LIB_SOURCE_DIR}/os/win/pa_win_coinitialize.h
		${LIB_SOURCE_DIR}/os/win/pa_win_hostapis.c
		${LIB_SOURCE_DIR}/os/win/pa_win_util.c
		${LIB_SOURCE_DIR}/os/win/pa_win_waveformat.c
		${LIB_SOURCE_DIR}/os/win/pa_win_wdmks_utils.c
		${LIB_SOURCE_DIR}/os/win/pa_win_wdmks_utils.h
		${LIB_SOURCE_DIR}/os/win/pa_x86_plain_converters.c
		${LIB_SOURCE_DIR}/os/win/pa_x86_plain_converters.h
	)

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/common)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/os/win)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PORTAUDIO=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC RTAUDIO_DUMMY=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC MACOSX_CORE=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_asiosdk)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()

    if (WIN32)
        # target_compile_definitions(${LIB_NAME} PUBLIC _WIN32_WINNT=0x0602)
        target_link_libraries(${LIB_NAME} PRIVATE dsound ole32 uuid rpcrt4 oleaut32 winmm)
        target_link_libraries(${LIB_NAME} PRIVATE mfplat mf mfuuid wmcodecdspuuid ksuser ks ksguid)
        target_link_libraries(${LIB_NAME} PRIVATE advapi32 comctl32 kernel32 user32)
        target_link_libraries(${LIB_NAME} PRIVATE mf3216 mapi32 mapistub mdminst imm32)

        target_compile_definitions(${LIB_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC PAWIN_USE_WDMKS_DEVICE_INFO=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE=0) # Linker Errors on MinGW
        target_compile_definitions(${LIB_NAME} PUBLIC PA_WDMKS_NO_KSGUID_LIB=1)
        target_compile_definitions(${LIB_NAME} PUBLIC PA_USE_ASIO=1)
        target_compile_definitions(${LIB_NAME} PUBLIC PA_USE_WMME=1)
        target_compile_definitions(${LIB_NAME} PUBLIC PA_USE_DS=1)
        target_compile_definitions(${LIB_NAME} PUBLIC PA_USE_WASAPI=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC PA_USE_WDMKS=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC PA_ENABLE_DEBUG_OUTPUT=0)

        # target_compile_definitions(${LIB_NAME} PUBLIC _WIN32_WINNT=0x0602)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_ASIO__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_DS__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_WASAPI__=1)
    else()
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_ALSA__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __UNIX_JACK__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_PULSE__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_OSS__=1)
        target_link_libraries(${LIB_NAME} PUBLIC asound)        
        target_link_libraries(${LIB_NAME} PUBLIC jack)
        # target_link_libraries(${LIB_NAME} PUBLIC de_sndfile)
    endif()

endif()

# add_subdirectory(test)
