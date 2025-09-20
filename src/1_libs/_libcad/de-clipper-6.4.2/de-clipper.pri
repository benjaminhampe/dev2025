# Define the library name
LIB_NAME = de_clipper

# Include directories
LIB_HEADER_DIR = $$PWD/include/clipper
LIB_SOURCE_DIR = $$PWD/src

# Define headers and sources
LIB_HEADERS += $$LIB_HEADER_DIR/clipper.hpp
LIB_SOURCES += $$LIB_SOURCE_DIR/clipper.cpp

# Add the include path
INCLUDEPATH += $$LIB_HEADER_DIR/..

# Add the headers and sources to the project
HEADERS += $$LIB_HEADERS
SOURCES += $$LIB_SOURCES

# Handle static library
TEMPLATE = lib
CONFIG += staticlib

# For copying headers (if needed for SDK)
# QMAKE_POST_LINK += $$QMAKE_COPY $$CLIPPER_HEADERS $$CLIPPER_INCLUDE_DIR
