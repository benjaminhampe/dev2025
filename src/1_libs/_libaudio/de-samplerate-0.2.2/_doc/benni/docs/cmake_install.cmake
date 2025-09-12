# Install script for directory: D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/libsamplerate")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/sdk/mingw64_1310/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/libsamplerate" TYPE FILE FILES
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/api.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/api_callback.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/api_full.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/api_misc.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/api_simple.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/bugs.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/download.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/faq.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/history.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/index.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/license.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/lists.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/quality.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/win32.md"
    "D:/dev/_benni_dev/src/de-samplerate-0.2.2/docs/SRC.png"
    )
endif()

