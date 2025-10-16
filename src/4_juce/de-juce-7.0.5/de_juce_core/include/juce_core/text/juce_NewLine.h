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
#include <juce_core/text/juce_String.h>
#include <juce_core/text/juce_StringRef.h>

namespace juce
{
//   struct String;
//   struct StringRef;

//==============================================================================
/** This class is used for represent a new-line character sequence.

    To write a new-line to a stream, you can use the predefined 'newLine' variable, e.g.
    @code
    myOutputStream << "Hello World" << newLine << newLine;
    @endcode

    The exact character sequence that will be used for the new-line can be set and
    retrieved with OutputStream::setNewLineString() and OutputStream::getNewLineString().

    @tags{Core}
*/
class JUCE_API  NewLine
{
public:
    /** Returns the default new-line sequence that the library uses.
        @see OutputStream::setNewLineString()
    */
    static const char* getDefault() noexcept;

    /** Returns the default new-line sequence that the library uses.
        @see getDefault()
    */
    operator String() const;

    /** Returns the default new-line sequence that the library uses.
        @see OutputStream::setNewLineString()
    */
    operator StringRef() const noexcept;
};

//==============================================================================
/** A predefined object representing a new-line, which can be written to a string or stream.

    To write a new-line to a stream, you can use the predefined 'newLine' variable like this:
    @code
    myOutputStream << "Hello World" << newLine << newLine;
    @endcode
*/
extern NewLine newLine;

//==============================================================================
/** Writes a new-line sequence to a string.
    You can use the predefined object 'newLine' to invoke this, e.g.
    @code
    myString << "Hello World" << newLine << newLine;
    @endcode
*/
String& operator<< (String& string1, const NewLine&);
String& operator+= (String& s1, const NewLine&);

String operator+ (const NewLine&, const NewLine&);
String operator+ (String s1, const NewLine&);
String operator+ (const NewLine&, const char* s2);

   
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4514 4996)

#if defined (JUCE_STRINGS_ARE_UNICODE) && ! JUCE_STRINGS_ARE_UNICODE
 #error "JUCE_STRINGS_ARE_UNICODE is deprecated! All strings are now unicode by default."
#endif

#if JUCE_NATIVE_WCHAR_IS_UTF8
 using CharPointer_wchar_t = CharPointer_UTF8;
#elif JUCE_NATIVE_WCHAR_IS_UTF16
 using CharPointer_wchar_t = CharPointer_UTF16;
#else
 using CharPointer_wchar_t = CharPointer_UTF32;
#endif

static CharPointer_wchar_t castToCharPointer_wchar_t (const void* t) noexcept
{
    return CharPointer_wchar_t (static_cast<const CharPointer_wchar_t::CharType*> (t));
}

//==============================================================================
struct StringHolder
{
    using CharPointerType  = String::CharPointerType;
    using CharType         = String::CharPointerType::CharType;

    std::atomic<int> refCount { 0 };
    size_t allocatedNumBytes = sizeof (CharType);
    CharType text[1] { 0 };
};

constexpr StringHolder emptyString;

//==============================================================================
class StringHolderUtils
{
public:
    using CharPointerType = StringHolder::CharPointerType;
    using CharType        = StringHolder::CharType;

    static CharPointerType createUninitialisedBytes (size_t numBytes)
    {
        numBytes = (numBytes + 3) & ~(size_t) 3;
        auto* bytes = new char [sizeof (StringHolder) - sizeof (CharType) + numBytes];
        auto s = unalignedPointerCast<StringHolder*> (bytes);
        s->refCount = 0;
        s->allocatedNumBytes = numBytes;
        return CharPointerType (unalignedPointerCast<CharType*> (bytes + offsetof (StringHolder, text)));
    }

    template <class CharPointer>
    static CharPointerType createFromCharPointer (const CharPointer text)
    {
        if (text.getAddress() == nullptr || text.isEmpty())
            return CharPointerType (emptyString.text);

        auto bytesNeeded = sizeof (CharType) + CharPointerType::getBytesRequiredFor (text);
        auto dest = createUninitialisedBytes (bytesNeeded);
        CharPointerType (dest).writeAll (text);
        return dest;
    }

    template <class CharPointer>
    static CharPointerType createFromCharPointer (const CharPointer text, size_t maxChars)
    {
        if (text.getAddress() == nullptr || text.isEmpty() || maxChars == 0)
            return CharPointerType (emptyString.text);

        auto end = text;
        size_t numChars = 0;
        size_t bytesNeeded = sizeof (CharType);

        while (numChars < maxChars && ! end.isEmpty())
        {
            bytesNeeded += CharPointerType::getBytesRequiredFor (end.getAndAdvance());
            ++numChars;
        }

        auto dest = createUninitialisedBytes (bytesNeeded);
        CharPointerType (dest).writeWithCharLimit (text, (int) numChars + 1);
        return dest;
    }

    template <class CharPointer>
    static CharPointerType createFromCharPointer (const CharPointer start, const CharPointer end)
    {
        if (start.getAddress() == nullptr || start.isEmpty())
            return CharPointerType (emptyString.text);

        auto e = start;
        int numChars = 0;
        auto bytesNeeded = sizeof (CharType);

        while (e < end && ! e.isEmpty())
        {
            bytesNeeded += CharPointerType::getBytesRequiredFor (e.getAndAdvance());
            ++numChars;
        }

        auto dest = createUninitialisedBytes (bytesNeeded);
        CharPointerType (dest).writeWithCharLimit (start, numChars + 1);
        return dest;
    }

    static CharPointerType createFromCharPointer (const CharPointerType start, const CharPointerType end)
    {
        if (start.getAddress() == nullptr || start.isEmpty())
            return CharPointerType (emptyString.text);

        auto numBytes = (size_t) (reinterpret_cast<const char*> (end.getAddress())
                                   - reinterpret_cast<const char*> (start.getAddress()));
        auto dest = createUninitialisedBytes (numBytes + sizeof (CharType));
        memcpy (dest.getAddress(), start, numBytes);
        dest.getAddress()[numBytes / sizeof (CharType)] = 0;
        return dest;
    }

    static CharPointerType createFromFixedLength (const char* const src, const size_t numChars)
    {
        auto dest = createUninitialisedBytes (numChars * sizeof (CharType) + sizeof (CharType));
        CharPointerType (dest).writeWithCharLimit (CharPointer_UTF8 (src), (int) (numChars + 1));
        return dest;
    }

    //==============================================================================
    static void retain (const CharPointerType text) noexcept
    {
        auto* b = bufferFromText (text);

        if (! isEmptyString (b))
            ++(b->refCount);
    }

    static void release (StringHolder* const b) noexcept
    {
        if (! isEmptyString (b))
            if (--(b->refCount) == -1)
                delete[] reinterpret_cast<char*> (b);
    }

    static void release (const CharPointerType text) noexcept
    {
        release (bufferFromText (text));
    }

    static int getReferenceCount (const CharPointerType text) noexcept
    {
        return bufferFromText (text)->refCount + 1;
    }

    //==============================================================================
    static CharPointerType makeUniqueWithByteSize (const CharPointerType text, size_t numBytes)
    {
        auto* b = bufferFromText (text);

        if (isEmptyString (b))
        {
            auto newText = createUninitialisedBytes (numBytes);
            newText.writeNull();
            return newText;
        }

        if (b->allocatedNumBytes >= numBytes && b->refCount <= 0)
            return text;

        auto newText = createUninitialisedBytes (jmax (b->allocatedNumBytes, numBytes));
        memcpy (newText.getAddress(), text.getAddress(), b->allocatedNumBytes);
        release (b);

        return newText;
    }

    static size_t getAllocatedNumBytes (const CharPointerType text) noexcept
    {
        return bufferFromText (text)->allocatedNumBytes;
    }

private:
    StringHolderUtils() = delete;
    ~StringHolderUtils() = delete;

    static StringHolder* bufferFromText (const CharPointerType charPtr) noexcept
    {
        return unalignedPointerCast<StringHolder*> (unalignedPointerCast<char*> (charPtr.getAddress()) - offsetof (StringHolder, text));
    }

    static bool isEmptyString (StringHolder* other)
    {
        return other == &emptyString;
    }

    void compileTimeChecks()
    {
        // Let me know if any of these assertions fail on your system!
       #if JUCE_NATIVE_WCHAR_IS_UTF8
        static_assert (sizeof (wchar_t) == 1, "JUCE_NATIVE_WCHAR_IS_* macro has incorrect value");
       #elif JUCE_NATIVE_WCHAR_IS_UTF16
        static_assert (sizeof (wchar_t) == 2, "JUCE_NATIVE_WCHAR_IS_* macro has incorrect value");
       #elif JUCE_NATIVE_WCHAR_IS_UTF32
        static_assert (sizeof (wchar_t) == 4, "JUCE_NATIVE_WCHAR_IS_* macro has incorrect value");
       #else
        #error "native wchar_t size is unknown"
       #endif
    }
};
   
} // namespace juce
