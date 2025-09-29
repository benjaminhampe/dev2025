#pragma once
#include <stdarg.h>
#include <vector>

#include <common.hpp>


namespace rack {
/** Supplemental `std::string` functions */
namespace string {


/** Converts a printf format string and optional arguments into a std::string.
The wrapper template function below automatically converts all arguments (including format string) from `std::string` to `const char*` as needed.
*/
__attribute__((format(printf, 1, 2)))
RACK_DLL_API std::string RACK_DLL_CALL f(const char* format, ...);
RACK_DLL_API std::string RACK_DLL_CALL fV(const char* format, va_list args);

// Converts std::string arguments of f() to `const char*`
template<typename T>
T convertFArg(const T& t) {return t;}
inline const char* convertFArg(const std::string& s) {return s.c_str();}
template<typename... Args>
std::string f(Args... args) {
	// Allows accessing the original f() above
	typedef std::string (*FType)(const char* format, ...);
	return FType(f)(convertFArg(args)...);
}

/** Replaces all characters to lowercase letters */
RACK_DLL_API std::string RACK_DLL_CALL lowercase(const std::string& s);
/** Replaces all characters to uppercase letters */
RACK_DLL_API std::string RACK_DLL_CALL uppercase(const std::string& s);
/** Removes whitespace from beginning and end of string. */
RACK_DLL_API std::string RACK_DLL_CALL trim(const std::string& s);
/** Truncates a string to not exceed a number of UTF-8 codepoints. */
RACK_DLL_API std::string RACK_DLL_CALL truncate(const std::string& s, size_t maxCodepoints);
/** Truncates the beginning of a string to not exceed a number of UTF-8 codepoints. */
RACK_DLL_API std::string RACK_DLL_CALL truncatePrefix(const std::string& s, size_t maxCodepoints);
/** Truncates and adds "…" to the end of a string, to not exceed a number of UTF-8 codepoints. */
RACK_DLL_API std::string RACK_DLL_CALL ellipsize(const std::string& s, size_t maxCodepoints);
/** Truncates and adds "…" to the beginning of a string, to not exceed a number of UTF-8 codepoints. */
RACK_DLL_API std::string RACK_DLL_CALL ellipsizePrefix(const std::string& s, size_t maxCodepoints);
/** Returns whether a string starts with the given substring. */
RACK_DLL_API bool RACK_DLL_CALL startsWith(const std::string& str, const std::string& prefix);
/** Returns whether a string ends with the given substring. */
RACK_DLL_API bool RACK_DLL_CALL endsWith(const std::string& str, const std::string& suffix);

/** Converts a byte array to a Base64-encoded string.
https://en.wikipedia.org/wiki/Base64
*/
RACK_DLL_API std::string RACK_DLL_CALL toBase64(const uint8_t* data, size_t dataLen);
RACK_DLL_API std::string RACK_DLL_CALL toBase64(const std::vector<uint8_t>& data);
/** Converts a Base64-encoded string to a byte array.
Throws std::runtime_error if string is invalid.
*/
RACK_DLL_API std::vector<uint8_t> RACK_DLL_CALL fromBase64(const std::string& str);

struct RACK_DLL_API CaseInsensitiveCompare {
	/** Returns whether `a < b` using case-insensitive lexical comparison. */
	bool operator()(const std::string& a, const std::string& b) const;
};

/** Joins an container (vector, list, etc) of std::strings with an optional separator string.
*/
template <typename TContainer>
std::string join(const TContainer& container, std::string seperator = "") {
	std::string s;
	bool first = true;
	for (const auto& c : container) {
		if (!first)
			s += seperator;
		first = false;
		s += c;
	}
	return s;
}

/** Splits a string into a vector of tokens.
If `maxTokens > 0`, limits the number of tokens.
Tokens do not include the separator string.
Examples:
	split("a+b+c", "+") // {"a", "b", "c"}
	split("abc", "+") // {"abc"}
	split("a++c", "+") // {"a", "", "c"}
	split("", "+") // {}
	split("abc", "") // throws rack::Exception
*/
RACK_DLL_API std::vector<std::string> RACK_DLL_CALL split(const std::string& s, const std::string& seperator, size_t maxTokens = 0);

/** Formats a UNIX timestamp with a strftime() string. */
RACK_DLL_API std::string RACK_DLL_CALL formatTime(const char* format, double timestamp);
RACK_DLL_API std::string RACK_DLL_CALL formatTimeISO(double timestamp);

// Unicode functions
/** Converts a UTF-32 string to a UTF-8 string.
Skips invalid UTF-32 codepoints (greater than 0x10FFFF).
*/
RACK_DLL_API std::string RACK_DLL_CALL UTF32toUTF8(const std::u32string& s32);
/** Converts a UTF-8 string to a UTF-32 string.
Skips invalid, overlong, and surrogate pair UTF-8 sequences.
*/
RACK_DLL_API std::u32string RACK_DLL_CALL UTF8toUTF32(const std::string& s8);
/** Finds the byte position of the next codepoint in a valid UTF-8 string.
pos is the byte position of the start of a codepoint.
Returns s8.size() if given codepoint is the last.
*/
RACK_DLL_API size_t RACK_DLL_CALL UTF8NextCodepoint(const std::string& s8, size_t pos);
/** Finds the byte position of the previous codepoint in a valid UTF-8 string.
pos is the byte position of the start of a codepoint.
Returns 0 if given codepoint is the first.
*/
RACK_DLL_API size_t RACK_DLL_CALL UTF8PrevCodepoint(const std::string& s8, size_t pos);
/** Returns the number of codepoints in a valid UTF-8 string.
O(len) time
*/
RACK_DLL_API size_t RACK_DLL_CALL UTF8Length(const std::string& s8);
/** Returns a codepoint's index in a valid UTF-8 string.
pos is the byte position of the start of a codepoint.
O(pos) time
*/
RACK_DLL_API size_t RACK_DLL_CALL UTF8CodepointIndex(const std::string& s8, size_t pos);
/** Returns a codepoint's byte position in a valid UTF-8 string.
Returns s8.size() if index is beyond the last codepoint.
O(index) time
*/
RACK_DLL_API size_t RACK_DLL_CALL UTF8CodepointPos(const std::string& s8, size_t index);

#ifdef _WIN32
/** Performs a Unicode string conversion from UTF-16 to UTF-8.
These are only defined on Windows because the implementation uses Windows' API, and conversion is not needed on other OS's (since everything on Mac and Linux is UTF-8).

std::string and char* variables are considered UTF-8, anywhere in the program.
See https://utf8everywhere.org/ for more information about VCV Rack's philosophy on string encoding, especially section 10 for rules VCV follows for handling text on Windows.
*/
RACK_DLL_API std::string RACK_DLL_CALL UTF16toUTF8(const std::wstring& w);
RACK_DLL_API std::wstring RACK_DLL_CALL UTF8toUTF16(const std::string& s);
#endif


/** Structured version string, for comparison.

Strings are split into parts by "." and compared lexicographically.
Parts are compared as the following.

If both parts can be parsed as integers (such as "10" and "2"), their integer value is compared (so "10" is greater).
If one part cannot be parsed as an integer (such as "2" and "beta"), the integer part is greater ("2").
If neither part can be parsed as an integer (such as "alpha" and "beta"), the parts are compared as strings in typical lexicographical order.

For example, the following versions are sorted earliest to latest.

	1.a.0
	1.b.0
	1.0.0
	1.0.1
	1.2.0
	1.10.0
	2.0.0
*/
struct RACK_DLL_API Version {
	std::vector<std::string> parts;

	Version() {}
	Version(const std::string& s);
	Version(const char* s) : Version(std::string(s)) {}
	operator std::string() const;
	/** Returns whether this version is earlier than `other`. */
	bool operator<(const Version& other);

	std::string getMajor() const {
		return get(parts, 0, "");
	}
	std::string getMinor() const {
		return get(parts, 1, "");
	}
	std::string getRevision() const {
		return get(parts, 2, "");
	}
};


/** Returns translation string of the current language setting from `translations/<language>.json`, or English if not found. */
RACK_DLL_API std::string RACK_DLL_CALL translate(const std::string& id);
/** Returns translation string of the given language, or "" if not found. */
RACK_DLL_API std::string RACK_DLL_CALL translate(const std::string& id, const std::string& language);
/** Returns ISO 639-1 language codes of loaded translations, sorted by name of language. */
std::vector<std::string> getLanguages();

RACK_DLL_API void RACK_DLL_CALL init();


} // namespace string
} // namespace rack
