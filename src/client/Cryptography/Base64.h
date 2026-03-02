#pragma once

#include <string>
#include <base64.h>

std::string b64encode(const std::string& source);
std::string b64decode(const std::string& source);
