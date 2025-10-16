set(LIB_NAME de_juce_events)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_events)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_events)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_events.h
      ${LIB_HEADER_DIR}/juce_events_config.h
      ${LIB_HEADER_DIR}/broadcasters/juce_ActionBroadcaster.h
      ${LIB_HEADER_DIR}/broadcasters/juce_ActionListener.h
      ${LIB_HEADER_DIR}/broadcasters/juce_AsyncUpdater.h
      ${LIB_HEADER_DIR}/broadcasters/juce_ChangeBroadcaster.h
      ${LIB_HEADER_DIR}/broadcasters/juce_ChangeListener.h
      ${LIB_HEADER_DIR}/interprocess/juce_ConnectedChildProcess.h
      ${LIB_HEADER_DIR}/interprocess/juce_InterprocessConnection.h
      ${LIB_HEADER_DIR}/interprocess/juce_InterprocessConnectionServer.h
      ${LIB_HEADER_DIR}/interprocess/juce_NetworkServiceDiscovery.h
      ${LIB_HEADER_DIR}/messages/juce_ApplicationBase.h
      ${LIB_HEADER_DIR}/messages/juce_CallbackMessage.h
      ${LIB_HEADER_DIR}/messages/juce_DeletedAtShutdown.h
      ${LIB_HEADER_DIR}/messages/juce_Initialisation.h
      ${LIB_HEADER_DIR}/messages/juce_Message.h
      ${LIB_HEADER_DIR}/messages/juce_MessageListener.h
      ${LIB_HEADER_DIR}/messages/juce_MessageManager.h
      ${LIB_HEADER_DIR}/messages/juce_MountedVolumeListChangeDetector.h
      ${LIB_HEADER_DIR}/messages/juce_NotificationType.h
      ${LIB_HEADER_DIR}/native/juce_ScopedLowPowerModeDisabler.h
      ${LIB_HEADER_DIR}/native/juce_win32_HiddenMessageWindow.h
      #${LIB_HEADER_DIR}/native/juce_win32_WinRTWrapper.h
      ${LIB_HEADER_DIR}/timers/juce_MultiTimer.h
      ${LIB_HEADER_DIR}/timers/juce_Timer.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_events.cpp
      ${LIB_SOURCE_DIR}/broadcasters/juce_ActionBroadcaster.cpp
      ${LIB_SOURCE_DIR}/broadcasters/juce_AsyncUpdater.cpp
      ${LIB_SOURCE_DIR}/broadcasters/juce_ChangeBroadcaster.cpp
      ${LIB_SOURCE_DIR}/interprocess/juce_ConnectedChildProcess.cpp
      ${LIB_SOURCE_DIR}/interprocess/juce_InterprocessConnection.cpp
      ${LIB_SOURCE_DIR}/interprocess/juce_InterprocessConnectionServer.cpp
      ${LIB_SOURCE_DIR}/interprocess/juce_NetworkServiceDiscovery.cpp
      ${LIB_SOURCE_DIR}/messages/juce_ApplicationBase.cpp
      ${LIB_SOURCE_DIR}/messages/juce_DeletedAtShutdown.cpp
      ${LIB_SOURCE_DIR}/messages/juce_MessageListener.cpp
      ${LIB_SOURCE_DIR}/messages/juce_MessageManager.cpp
      #${LIB_SOURCE_DIR}/native/juce_android_Messaging.cpp
      #${LIB_SOURCE_DIR}/native/juce_linux_Messaging.cpp
      ${LIB_SOURCE_DIR}/native/juce_ScopedLowPowerModeDisabler.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_Messaging.cpp
      #${LIB_SOURCE_DIR}/native/juce_win32_WinRTWrapper.cpp
      ${LIB_SOURCE_DIR}/timers/juce_MultiTimer.cpp
      ${LIB_SOURCE_DIR}/timers/juce_Timer.cpp
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_core)

endif()

# add_subdirectory(test)

