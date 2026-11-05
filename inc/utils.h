#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <stdexcept>

#include "constants.h"

// ==========================================
// CORE STRING_VIEW OVERLOADS
// ==========================================

// 1. string_view + string_view (The one you rightfully pointed out!)
inline std::string operator+(std::string_view lhs, std::string_view rhs)
{
	std::string result;
	result.reserve(lhs.size() + rhs.size());
	result.append(lhs).append(rhs);
	return result;
}

// 2. string_view + char literal (e.g., s1 + ".")
inline std::string operator+(std::string_view lhs, const char* rhs)
{
	return lhs + std::string_view(rhs);
}

// 3. char literal + string_view (e.g., "." + s2)
inline std::string operator+(const char* lhs, std::string_view rhs)
{
	return std::string_view(lhs) + rhs;
}

// ==========================================
// SINGLE CHARACTER LITERALS (e.g., s1 + '.')
// ==========================================

// 4. string_view + char
inline std::string operator+(std::string_view lhs, char rhs)
{
	std::string result;
	result.reserve(lhs.size() + 1);
	result.append(lhs).push_back(rhs);
	return result;
}

// 5. char + string_view
inline std::string operator+(char lhs, std::string_view rhs)
{
	std::string result;
	result.reserve(1 + rhs.size());
	result.push_back(lhs);
	result.append(rhs);
	return result;
}

// ==========================================
// CHAINING ACCUMULATORS (std::string temporaries)
// ==========================================

// 6. std::string (temporary) + string_view
inline std::string operator+(std::string&& lhs, std::string_view rhs)
{
	lhs.append(rhs);
	return std::move(lhs);
}

// 7. std::string (temporary) + char
inline std::string operator+(std::string&& lhs, char rhs)
{
	lhs.push_back(rhs);
	return std::move(lhs);
}

// 8. std::string (temporary) + const char* literal
inline std::string operator+(std::string&& lhs, const char* rhs)
{
	lhs.append(rhs);
	return std::move(lhs);
}

// ==========================================
// RUNTIME BASE (std::string at the very beginning)
// ==========================================

// 9. std::string (lvalue) + string_view
inline std::string operator+(const std::string& lhs, std::string_view rhs)
{
	std::string result;
	result.reserve(lhs.size() + rhs.size());
	result.append(lhs).append(rhs);
	return result;
}

// clang-format off
constexpr std::string_view SPC  = " ";
constexpr std::string_view SEP  = ", ";
constexpr std::string_view PFX  = ": ";
constexpr std::string_view SFX  = ";";
constexpr std::string_view DOT  = ".";
constexpr std::string_view TAG  = "'";
constexpr std::string_view LP   = "(";
constexpr std::string_view RP   = ")";
constexpr std::string_view LB   = "[";
constexpr std::string_view RB   = "]";
constexpr std::string_view QT   = "\"";
constexpr std::string_view NL   = "\n";
constexpr std::string_view DASH = " - ";
constexpr std::string_view USC = "_";
constexpr std::string_view TAB  = "       ";
constexpr std::string_view ERR_PFX  = "Error: ";
constexpr std::string_view WARN_PFX = "Warning: ";
constexpr std::string_view INFO_PFX = "Info: ";
// clang-format on

#define BOOL(b) ((b) ? "true" : "false")

#define ENDL      std::endl
#define FLUSH     std::flush
#define FUNC      STR(__PRETTY_FUNCTION__, 1) << SFX
#define HEX(n, w) "0x" << std::hex << std::setw((w)) << std::setfill('0') << std::right << (n)
#define DEC(n, w) std::dec << std::setw((w)) << std::setfill(' ') << std::right << (n)
#define FLT(n, p) std::setprecision((p)) << std::fixed << (n)
#define STR(s, w) std::setw((w)) << std::setfill(' ') << std::left << (s)
#define ERR(s)    std::cerr << STR(ERR_PFX, 1) << s << PFX << FUNC << ENDL << FLUSH
#define WARN(s)   std::cerr << STR(WARN_PFX, 1) << s << PFX << FUNC << ENDL << FLUSH
#define INFO(s)   std::cout << STR(INFO_PFX, 1) << s << ENDL << FLUSH
#define PRN(s)    std::cout << s /*<< ENDL*/ << FLUSH

#ifdef DEBUG_PRINTS
constexpr std::string_view DBG_PFX = "Debug: ";
#define DBG(s) std::cout << STR(DBG_PFX, 1) << s << ENDL << FLUSH
#else
#define DBG(...)
#endif // DEBUG_PRINTS

struct system_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

system_error sys_error(const char* s1, const char* s2 = nullptr);
