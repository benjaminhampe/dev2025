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

#include <juce_core/streams/juce_FileInputSource.h>
#include <juce_core/files/juce_FileInputStream.h>
#include <juce_core/time/juce_Time.h>

namespace juce
{

FileInputSource::FileInputSource (const File& f, bool useFileTimeInHash)
    : file (f), useFileTimeInHashGeneration (useFileTimeInHash)
{
}

FileInputSource::~FileInputSource()
{
}

InputStream* FileInputSource::createInputStream()
{
    return file.createInputStream().release();
}

InputStream* FileInputSource::createInputStreamFor (const String& relatedItemPath)
{
    return file.getSiblingFile (relatedItemPath).createInputStream().release();
}

int64 FileInputSource::hashCode() const
{
    int64 h = file.hashCode();

    if (useFileTimeInHashGeneration)
        h ^= file.getLastModificationTime().toMilliseconds();

    return h;
}

} // namespace juce
