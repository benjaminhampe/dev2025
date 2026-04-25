#pragma once
#include <cstdint>
#include <string>

namespace de {

// A time string in ISO 8601 format: fromNanoseconds()
struct ISO8601
{
    std::string d;
    std::string t;
    std::string ns;

    std::string str(bool bDate = true, bool bTime = true, bool bNanos = true) const;

    static ISO8601 fromNanoseconds(int64_t ns);
};

} // end namespace de