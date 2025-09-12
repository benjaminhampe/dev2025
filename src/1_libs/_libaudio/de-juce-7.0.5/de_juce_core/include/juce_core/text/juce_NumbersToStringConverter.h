#pragma once
#include <juce_core/text/juce_String.h>
#include <juce_core/logging/juce_Logger.h> // jassert impl

namespace juce
{

//==============================================================================
namespace NumberToStringConverters
{
    enum
    {
        charsNeededForInt = 32,
        charsNeededForDouble = 48
    };

    template <typename Type>
    static char* printDigits (char* t, Type v) noexcept
    {
        *--t = 0;

        do
        {
            *--t = static_cast<char> ('0' + (char) (v % 10));
            v /= 10;

        } while (v > 0);

        return t;
    }

    // pass in a pointer to the END of a buffer..
    static char* numberToString (char* t, int64 n) noexcept
    {
        if (n >= 0)
            return printDigits (t, static_cast<uint64> (n));

        // NB: this needs to be careful not to call -std::numeric_limits<int64>::min(),
        // which has undefined behaviour
        t = printDigits (t, static_cast<uint64> (-(n + 1)) + 1);
        *--t = '-';
        return t;
    }

    static char* numberToString (char* t, uint64 v) noexcept
    {
        return printDigits (t, v);
    }

    static char* numberToString (char* t, int n) noexcept
    {
        if (n >= 0)
            return printDigits (t, static_cast<unsigned int> (n));

        // NB: this needs to be careful not to call -std::numeric_limits<int>::min(),
        // which has undefined behaviour
        t = printDigits (t, static_cast<unsigned int> (-(n + 1)) + 1);
        *--t = '-';
        return t;
    }

    static char* numberToString (char* t, unsigned int v) noexcept
    {
        return printDigits (t, v);
    }

    static char* numberToString (char* t, long n) noexcept
    {
        if (n >= 0)
            return printDigits (t, static_cast<unsigned long> (n));

        t = printDigits (t, static_cast<unsigned long> (-(n + 1)) + 1);
        *--t = '-';
        return t;
    }

    static char* numberToString (char* t, unsigned long v) noexcept
    {
        return printDigits (t, v);
    }

    struct StackArrayStream  : public std::basic_streambuf<char, std::char_traits<char>>
    {
        explicit StackArrayStream (char* d)
        {
            static const std::locale classicLocale (std::locale::classic());
            imbue (classicLocale);
            setp (d, d + charsNeededForDouble);
        }

        size_t writeDouble (double n, int numDecPlaces, bool useScientificNotation)
        {
            {
                std::ostream o (this);

                if (numDecPlaces > 0)
                {
                    o.setf (useScientificNotation ? std::ios_base::scientific : std::ios_base::fixed);
                    o.precision ((std::streamsize) numDecPlaces);
                }

                o << n;
            }

            return (size_t) (pptr() - pbase());
        }
    };

    static char* doubleToString (char* buffer, double n, int numDecPlaces, bool useScientificNotation, size_t& len) noexcept
    {
        StackArrayStream strm (buffer);
        len = strm.writeDouble (n, numDecPlaces, useScientificNotation);
        jassert (len <= charsNeededForDouble);
        return buffer;
    }

    template <typename IntegerType>
    static String::CharPointerType createFromInteger (IntegerType number)
    {
        char buffer [charsNeededForInt];
        auto* end = buffer + numElementsInArray (buffer);
        auto* start = numberToString (end, number);
        return StringHolderUtils::createFromFixedLength (start, (size_t) (end - start - 1));
    }

    static String::CharPointerType createFromDouble (double number, int numberOfDecimalPlaces, bool useScientificNotation)
    {
        char buffer [charsNeededForDouble];
        size_t len;
        auto start = doubleToString (buffer, number, numberOfDecimalPlaces, useScientificNotation, len);
        return StringHolderUtils::createFromFixedLength (start, len);
    }
}

} // end namespace juce

