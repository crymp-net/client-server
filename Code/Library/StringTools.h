#pragma once

#include <cstdarg>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

extern "C" __declspec(dllimport) int __stdcall MultiByteToWideChar(
	unsigned int codepage,
	unsigned long flags,
	const char* string, int stringByteCount,
	wchar_t* buffer, int bufferCharCount
);

extern "C" __declspec(dllimport) int __stdcall WideCharToMultiByte(
	unsigned int codepage,
	unsigned long flags,
	const wchar_t* string, int stringCharCount,
	char* buffer, int bufferByteCount,
	const char* defaultChar, int* defaultCharUsed
);

namespace StringTools
{
	template<typename T>
	using Char = std::remove_cv_t<std::remove_reference_t<decltype(T{}[0])>>;

	template<typename T>
	auto CStr(const T& string)
	{
		if constexpr (std::is_pointer_v<T> || std::is_array_v<T>)
		{
			return string;
		}
		else
		{
			return string.data();
		}
	}

	template<typename T>
	std::size_t Length(const T& string)
	{
		if constexpr (std::is_pointer_v<T> || std::is_array_v<T>)
		{
			return std::basic_string_view<Char<T>>(string).length();
		}
		else
		{
			return string.length();
		}
	}

	template<typename T>
	void ToLowerInPlace(T& string)
	{
		for (auto& ch : string)
		{
			if (ch >= 'A' && ch <= 'Z')
			{
				ch += ('a' - 'A');
			}
		}
	}

	template<typename T>
	void ToUpperInPlace(T& string)
	{
		for (auto& ch : string)
		{
			if (ch >= 'a' && ch <= 'z')
			{
				ch -= ('a' - 'A');
			}
		}
	}

	template<typename T>
	auto ToLower(const T& string)
	{
		std::basic_string<Char<T>> result(string);
		ToLowerInPlace(result);

		return result;
	}

	template<typename T>
	auto ToUpper(const T& string)
	{
		std::basic_string<Char<T>> result(string);
		ToUpperInPlace(result);

		return result;
	}

	inline std::string SafeString(const char* value)
	{
		return std::string(value ? value : "");
	}

	inline std::wstring SafeWString(const wchar_t* value)
	{
		return std::wstring(value ? value : L"");
	}

	inline std::string_view SafeView(const char* value)
	{
		return std::string_view(value ? value : "");
	}

	inline std::wstring_view SafeWView(const wchar_t* value)
	{
		return std::wstring_view(value ? value : L"");
	}

	namespace Detail
	{
		inline std::size_t RawUTF8ToWide(
			const char* string, std::size_t stringByteCount,
			wchar_t* buffer, std::size_t bufferCharCount
		){
			return MultiByteToWideChar(
				65001,  // CP_UTF8
				0,
				string, static_cast<int>(stringByteCount),
				buffer, static_cast<int>(bufferCharCount)
			);
		}

		inline std::size_t RawWideToUTF8(
			const wchar_t* string, std::size_t stringCharCount,
			char* buffer, std::size_t bufferByteCount
		){
			return WideCharToMultiByte(
				65001,  // CP_UTF8
				0,
				string, static_cast<int>(stringCharCount),
				buffer, static_cast<int>(bufferByteCount),
				nullptr, nullptr
			);
		}
	}

	template<typename T, typename U>
	void AppendTo(T& result, const U& string)
	{
		if constexpr (std::is_same_v<Char<T>, Char<U>>)
		{
			result.append(CStr(string), Length(string));
		}
		else
		{
			const std::size_t existingLength = result.length();
			const std::size_t stringLength = Length(string);

			const auto convert = [&](std::size_t bufferSize) -> std::size_t
			{
				constexpr bool UTF8_TO_WIDE = std::is_same_v<Char<U>, char>
				                           && std::is_same_v<Char<T>, wchar_t>;

				constexpr bool WIDE_TO_UTF8 = std::is_same_v<Char<T>, char>
				                           && std::is_same_v<Char<U>, wchar_t>;

				static_assert(UTF8_TO_WIDE || WIDE_TO_UTF8);

				auto buffer = const_cast<Char<T>*>(result.data() + existingLength);

				if (bufferSize == 0)
				{
					buffer = nullptr;
				}

				if constexpr (UTF8_TO_WIDE)
				{
					return Detail::RawUTF8ToWide(CStr(string), stringLength, buffer, bufferSize);
				}
				else
				{
					return Detail::RawWideToUTF8(CStr(string), stringLength, buffer, bufferSize);
				}
			};

			if (stringLength > 0)
			{
				const std::size_t bufferSize = stringLength;
				result.resize(existingLength + bufferSize);

				const std::size_t convertedLength = convert(bufferSize);
				if (convertedLength > 0)
				{
					result.resize(existingLength + convertedLength);
				}
				else
				{
					const std::size_t requiredBufferSize = convert(0);
					result.resize(existingLength + requiredBufferSize);

					convert(requiredBufferSize);
				}
			}
		}
	}

	template<typename T, typename U>
	void AssignTo(T& result, const U& string)
	{
		result.clear();
		AppendTo(result, string);
	}

	std::string Format(const char* format, ...);
	std::string FormatV(const char* format, va_list args);

	std::size_t FormatTo(std::string& result, const char* format, ...);
	std::size_t FormatToV(std::string& result, const char* format, va_list args);

	std::size_t FormatTo(char* buffer, std::size_t bufferSize, const char* format, ...);
	std::size_t FormatToV(char* buffer, std::size_t bufferSize, const char* format, va_list args);

	std::runtime_error ErrorFormat(const char* format, ...);
	std::runtime_error ErrorFormatV(const char* format, va_list args);

	std::system_error SysErrorFormat(const char* format, ...);
	std::system_error SysErrorFormatV(const char* format, va_list args);

	std::system_error SysErrorErrnoFormat(const char* format, ...);
	std::system_error SysErrorErrnoFormatV(const char* format, va_list args);
}
