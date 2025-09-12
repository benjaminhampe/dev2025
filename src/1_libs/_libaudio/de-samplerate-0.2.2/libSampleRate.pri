#####################
### libSampleRate ###
#####################
DEFINES += USE_LIBSAMPLERATE=1

LIBSAMPLERATE_DIR = $$PWD 
LIBSAMPLERATE_INC = $$PWD/include

INCLUDEPATH += $$LIBSAMPLERATE_INC

HEADERS += $$LIBSAMPLERATE_INC/samplerate.h

CONFIG(release, debug|release) {
	LIBS += -l$$LIBSAMPLERATE_DIR/$$TOOLSET/Release/libsamplerate-0
}
CONFIG(debug, debug|release) {
    LIBS += -l$$LIBSAMPLERATE_DIR/$$TOOLSET/Debug/libsamplerated-0
}