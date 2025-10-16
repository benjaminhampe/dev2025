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

#include <juce_core/text/juce_NewLine.h>
#include <juce_core/text/juce_String.h>
#include <juce_core/text/juce_StringRef.h>

namespace juce
{
//static 
const char* 
NewLine::getDefault() noexcept        { return "\r\n"; }

NewLine::operator String() const                         { return getDefault(); }

NewLine::operator StringRef() const noexcept             { return getDefault(); }

String& operator<< (String& string1, const NewLine&) { return string1 += NewLine::getDefault(); }
String& operator+= (String& s1, const NewLine&)      { return s1 += NewLine::getDefault(); }

String operator+ (const NewLine&, const NewLine&)    { return String (NewLine::getDefault()) + NewLine::getDefault(); }
String operator+ (String s1, const NewLine&)         { return s1 += NewLine::getDefault(); }
String operator+ (const NewLine&, const char* s2)    { return String (NewLine::getDefault()) + s2; }

} // namespace juce
