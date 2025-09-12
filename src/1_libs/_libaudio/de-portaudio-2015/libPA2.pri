###########################################################
### PortAudio2 from svn trunk and many changes by Benni ###
###########################################################

DEFINES += USE_PORTAUDIO=1
DEFINES += _CRT_SECURE_NO_WARNINGS
DEFINES += PAWIN_USE_WDMKS_DEVICE_INFO
DEFINES += PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
DEFINES += PA_WDMKS_NO_KSGUID_LIB
DEFINES += PA_USE_ASIO=1
DEFINES += PA_USE_WMME=1
DEFINES += PA_USE_DS=1
DEFINES += PA_USE_WASAPI=1
#DEFINES += PA_USE_WDMKS=1
#DEFINES += PA_ENABLE_DEBUG_OUTPUT=0

PA_DIR = $$PWD
PA_INC = $$PA_DIR/include
INCLUDEPATH += $$PA_INC
HEADERS += $$PA_INC/*.h

PA_COMMON = $$PA_DIR/src/common
INCLUDEPATH += $$PA_COMMON
SOURCES += $$PA_COMMON/pa_allocation.c
HEADERS += $$PA_COMMON/pa_allocation.h
SOURCES += $$PA_COMMON/pa_converters.c
HEADERS += $$PA_COMMON/pa_converters.h
SOURCES += $$PA_COMMON/pa_cpuload.c
HEADERS += $$PA_COMMON/pa_cpuload.h
SOURCES += $$PA_COMMON/pa_debugprint.c
HEADERS += $$PA_COMMON/pa_debugprint.h
SOURCES += $$PA_COMMON/pa_dither.c
HEADERS += $$PA_COMMON/pa_dither.h
HEADERS += $$PA_COMMON/pa_endianness.h
SOURCES += $$PA_COMMON/pa_front.c
HEADERS += $$PA_COMMON/pa_hostapi.h
HEADERS += $$PA_COMMON/pa_memorybarrier.h
SOURCES += $$PA_COMMON/pa_process.c
HEADERS += $$PA_COMMON/pa_process.h
SOURCES += $$PA_COMMON/pa_ringbuffer.c
HEADERS += $$PA_COMMON/pa_ringbuffer.h
SOURCES += $$PA_COMMON/pa_stream.c
HEADERS += $$PA_COMMON/pa_stream.h
HEADERS += $$PA_COMMON/pa_svnrevision.h
SOURCES += $$PA_COMMON/pa_trace.c
HEADERS += $$PA_COMMON/pa_trace.h
HEADERS += $$PA_COMMON/pa_types.h
HEADERS += $$PA_COMMON/pa_util.h

PA_WIN = $$PA_DIR/src/os/win
INCLUDEPATH += $$PA_WIN
SOURCES += $$PA_WIN/pa_win_coinitialize.c
SOURCES += $$PA_WIN/pa_win_coinitialize.h
SOURCES += $$PA_WIN/pa_win_hostapis.c
SOURCES += $$PA_WIN/pa_win_util.c
SOURCES += $$PA_WIN/pa_win_waveformat.c
SOURCES += $$PA_WIN/pa_win_wdmks_utils.c
HEADERS += $$PA_WIN/pa_win_wdmks_utils.h
SOURCES += $$PA_WIN/pa_x86_plain_converters.c
HEADERS += $$PA_WIN/pa_x86_plain_converters.h

PAPI = $$PA_DIR/src/hostapi

PAPI_ASIO = $$PAPI/asio
INCLUDEPATH += $$PAPI_ASIO
HEADERS += $$PAPI_ASIO/iasiothiscallresolver.h
SOURCES += $$PAPI_ASIO/iasiothiscallresolver.cpp
SOURCES += $$PAPI_ASIO/pa_asio.cpp

PAPI_WMME = $$PAPI/wmme
INCLUDEPATH += $$PAPI_WMME
SOURCES += $$PAPI_WMME/pa_win_wmme.c

PAPI_DS = $$PAPI/dsound
INCLUDEPATH += $$PAPI_DS
HEADERS += $$PAPI_DS/pa_win_ds_dynlink.h
SOURCES += $$PAPI_DS/pa_win_ds_dynlink.c
SOURCES += $$PAPI_DS/pa_win_ds.c

PAPI_WASAPI = $$PAPI/wasapi
INCLUDEPATH += $$PAPI_WASAPI
SOURCES += $$PAPI_WASAPI/pa_win_wasapi.c

#PAPI_WDMKS = $$PAPI/wdmks
#INCLUDEPATH += $$PAPI_WDMKS
#SOURCES += $$PAPI_WDMKS/pa_win_wdmks.c