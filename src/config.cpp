#include <algorithm>
#include <array>
#include <charconv>

#include "config_document.h"

namespace config_ns
{

std::string escape_control_chars(const std::string& source)
{
	constexpr char hex[] = "0123456789ABCDEF";

	std::string result;
	result.reserve(source.size());

	for (unsigned char ch : source) {
		switch (ch) {
		case '\n':
			result += "'\\n'";
			break;
		case '\r':
			result += "'\\r'";
			break;
		case '\t':
			result += "'\\t'";
			break;
		case '\0':
			result += "'\\0'";
			break;
		case 0x7F:
			result += "'DEL'";
			break;
		default:
			if (ch < 0x20) {
				result += "'\\x";
				result += hex[ch >> 4];
				result += hex[ch & 0x0f];
				result += "'";
			}
			else {
				result += static_cast<char>(ch);
			}
			break;
		}
	}

	return result;
}

void add_error(const std::string& err, errors_t& errs)
{
	errs.push_back(err);
}

void print_errors(const config_ns::errors_t errs, const std::string pfx)
{
	for (const auto& err : errs) {
		PRN(TAB + escape_control_chars(pfx) + escape_control_chars(err) + NL);
	}
}

std::string_view get_att_name(const uint32_t i)
{
	auto it = att_names.find(i);
	if (it != att_names.end()) {
		return it->second;
	}

	return std::string_view("");
}

std::string_view get_coord_name(const uint32_t i)
{
	auto it = coord_names.find(i);
	if (it != coord_names.end()) {
		return it->second;
	}

	return std::string_view("");
}

std::string set_precision(const unit val, const uint32_t digits)
{
	std::array<char, 100> buf;

	auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), val, std::chars_format::fixed, digits);

	if (ec == std::errc::value_too_large) {
		return "";
	}

	std::string s(buf.data(), ptr);
	return s;
}

// Detect ASCII control characters, including DEL.
static inline bool has_control_chars(const std::string& str)
{
	// clang-format off
	return std::any_of(str.begin(), str.end(),
			[](unsigned char ch) {
				return ch < 0x20 ||
				       ch == 0x7f;
			}
	);
	// clang-format on
}

// Field names: lowercase ASCII letters and underscore only.
static inline bool field_name_all_valid_chars(const std::string& str)
{
	// clang-format off
	return std::all_of(str.begin(), str.end(),
			[](char ch) {
				return (ch >= 'a' &&
					ch <= 'z') ||
					ch == '_';
			}
	);
	// clang-format on
}

// Field values: reject selected punctuation.
// All other characters, including UTF-8 bytes, are allowed.
static inline bool field_value_all_valid_chars(const std::string& str)
{
	// clang-format off
	static const std::string rejected_chars(
			JSON_FIELD_VALUES_REJECTED_CHARS
		);

	return std::none_of(str.begin(), str.end(),
			[](char ch) {
				return rejected_chars.find(ch) !=
					std::string::npos;
			}
	);
	// clang-format on
}

// Validate JSON field names.
bool is_valid_field_name(const std::string& field)
{
	return !field.empty() &&
	       !has_control_chars(field) &&
	       field_name_all_valid_chars(field);
}

// Validate identifier/reference values.
bool is_valid_field_value(const std::string& value)
{
	return !value.empty() &&
	       !has_control_chars(value) &&
	       field_value_all_valid_chars(value);
}

} // namespace config_ns
