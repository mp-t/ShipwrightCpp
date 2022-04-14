#include "StringHelper.h"

#include <cstdarg>
#include <memory>

namespace StringHelper
{

std::string Sprintf(const char* format, ...)
{
	constexpr std::size_t bufferSize = 32768;
	auto buffer = std::make_unique<char[]>(bufferSize);

	std::va_list va;
	va_start(va, format);
	vsprintf_s(buffer.get(), bufferSize, format, va);
	va_end(va);

	return std::string{buffer.get()};
}

}