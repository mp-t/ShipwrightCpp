#pragma once

#include <cstdint>

struct Vec3s
{
	std::int16_t x = 0;
	std::int16_t y = 0;
	std::int16_t z = 0;

	constexpr Vec3s() = default;

	constexpr Vec3s(const std::int16_t x, const std::int16_t y, const std::int16_t z) : x(x), y(y), z(z)
	{
	}
};