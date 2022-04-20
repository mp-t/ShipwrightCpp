#pragma once
#include <string>

namespace Ship {
	bool stob(const std::string& s, bool defaultVal = false);
	std::int32_t stoi(const std::string& s, std::int32_t defaultVal = 0);
	float stof(const std::string& s, float defaultVal = 1.0f);
	std::int64_t stoll(const std::string& s, std::int64_t defaultVal = 0);
}