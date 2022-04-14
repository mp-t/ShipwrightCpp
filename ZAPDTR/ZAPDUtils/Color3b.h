#pragma once

#include <cstdint>

struct Color3b
{
	std::uint8_t r = 0;
	std::uint8_t g = 0;
	std::uint8_t b = 0;

	constexpr Color3b() = default;

	constexpr Color3b(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b) : r(r), g(g), b(b)
	{
	}
};