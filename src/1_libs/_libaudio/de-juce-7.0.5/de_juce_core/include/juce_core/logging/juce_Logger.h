/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once
#include <juce_core/juce_core_common.h>

namespace juce
{
   struct String;
//==============================================================================
/**
    Acts as an application-wide logging class.

    A subclass of Logger can be created and passed into the Logger::setCurrentLogger
    method and this will then be used by all calls to writeToLog.

    The logger class also contains methods for writing messages to the debugger's
    output stream.

    @see FileLogger

    @tags{Core}
*/
class JUCE_API  Logger
{
public:
    //==============================================================================
    /** Destructor. */
    virtual ~Logger();

    //==============================================================================
    /** Sets the current logging class to use.

        Note that the object passed in will not be owned or deleted by the logger, so
        the caller must make sure that it is not deleted while still being used.
        A null pointer can be passed-in to reset the system to the default logger.
    */
    static void JUCE_CALLTYPE setCurrentLogger (Logger* newLogger) noexcept;

    /** Returns the current logger, or nullptr if no custom logger has been set. */
    static Logger* JUCE_CALLTYPE getCurrentLogger() noexcept;

    /** Writes a string to the current logger.

        This will pass the string to the logger's logMessage() method if a logger
        has been set.

        @see logMessage
    */
    static void JUCE_CALLTYPE writeToLog (const String& message);


    //==============================================================================
    /** Writes a message to the standard error stream.

        This can be called directly, or by using the DBG() macro in
        juce_PlatformDefs.h (which will avoid calling the method in non-debug builds).
    */
    static void JUCE_CALLTYPE outputDebugString (const String& text);


protected:
    //==============================================================================
    Logger();

    /** This is overloaded by subclasses to implement custom logging behaviour.
        @see setCurrentLogger
    */
    virtual void logMessage (const String& message) = 0;

private:
    static Logger* currentLogger;
};

#if JUCE_LOG_ASSERTIONS || JUCE_DEBUG
JUCE_API void JUCE_CALLTYPE
logAssertion (const char* const filename, const int lineNum) noexcept;
#endif

// implemented in win32_Threads.cpp
JUCE_API bool JUCE_CALLTYPE
juce_isRunningUnderDebugger() noexcept;

} // namespace juce
