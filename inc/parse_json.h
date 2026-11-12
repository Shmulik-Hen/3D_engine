#pragma once

#include <fstream>
#include <json/json.h>

#include "config_document.h"

namespace config_ns
{

bool parse_json(std::ifstream&, config_ns::AST&);

} // namespace config_ns
