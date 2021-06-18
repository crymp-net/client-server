#pragma once

#include <stdarg.h>
#include <string>

std::string Format(const char *format, ...);
std::string FormatV(const char *format, va_list args);
