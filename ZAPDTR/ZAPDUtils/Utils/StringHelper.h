#pragma once

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace StringHelper
{
	[[nodiscard]] constexpr std::vector<std::string> Split(std::string s, const std::string& delimiter)
	{
		std::vector<std::string> result;

		std::size_t pos = 0;
		std::string token;

		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			result.push_back(token);
			s.erase(0, pos + delimiter.length());
		}

		if (s.length() != 0)
		{
			result.push_back(s);
		}

		return result;
	}

	[[nodiscard]] constexpr std::string Strip(std::string s, const std::string& delimiter)
	{
		std::size_t pos = 0;
		std::string token;

		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			s.erase(pos, pos + delimiter.length());
		}

		return s;
	}

	constexpr void ReplaceOriginal(std::string& str, const std::string& from, const std::string& to)
	{
		std::size_t start_pos = str.find(from);

		while (start_pos != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos = str.find(from);
		}
	}

	[[nodiscard]] constexpr bool StartsWith(const std::string& s, const std::string& input)
	{
		return s.rfind(input, 0) == 0;
	}

	[[nodiscard]] std::string Sprintf(const char* format, ...);

	[[nodiscard]] constexpr bool IEquals(const std::string& a, const std::string& b)
	{
		return std::equal(a.begin(), a.end(), b.begin(), b.end(),
			[](const char a, const char b) { return std::tolower(a) == std::tolower(b); });
	}
}