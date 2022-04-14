#pragma once

struct Vec2f
{
	float x = 0;
	float y = 0;

	constexpr Vec2f() = default;

	constexpr Vec2f(const float x, const float y) : x(x), y(y)
	{
	}
};