#pragma once

struct Vec3f
{
	float x = 0;
	float y = 0;
	float z = 0;

	constexpr Vec3f() = default;

	constexpr Vec3f(const float x, const float y, const float z) : x(x), y(y), z(z)
	{
	}
};