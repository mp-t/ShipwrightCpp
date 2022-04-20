#pragma once

#include <cstdint>

void luslog(const char* file, std::int32_t line, std::int32_t logLevel, const char* msg);
void lusprintf(const char* file, std::int32_t line, std::int32_t logLevel, const char* fmt, ...);

#define LUS_LOG(level, msg) luslog(__FILE__, __LINE__, level, msg)

