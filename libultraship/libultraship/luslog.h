#pragma once

#include <cstdint>

void luslog(const char* file, int32_t line, int32_t logLevel, const char* msg);
void lusprintf(const char* file, int32_t line, int32_t logLevel, const char* fmt, ...);

#define LUS_LOG(level, msg) luslog(__FILE__, __LINE__, level, msg)

