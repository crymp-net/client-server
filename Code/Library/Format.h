#pragma once

#include <stdarg.h>
#include <string>

std::string Format(const char *format, ...);
std::string FormatV(const char *format, va_list args);
std::string ToUpper(const std::string_view& text);
void ToUpperFast(std::string& text);
std::string ToLower(const std::string_view& text);
void ToLowerFast(std::string& text);
