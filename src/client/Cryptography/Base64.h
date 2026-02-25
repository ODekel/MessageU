#pragma once

#include <string>
#include <base64.h>

std::string b64encode(const std::vector<unsigned char>& source);
std::vector<unsigned char> b64decode(const std::string& str);
