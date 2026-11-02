#include "utils.h"

system_error sys_error(const char* s1, const char* s2)
{
	std::string msg = std::string(s1);
	if (s2) {
		msg += PFX;
		msg += s2;
	}

	throw system_error(msg);
}
