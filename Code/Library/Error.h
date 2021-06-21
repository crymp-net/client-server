#pragma once

#include <string>
#include <exception>

class Error : public std::exception
{
	int m_code = 0;
	std::string m_message;
	std::string m_description;
	std::string m_what;

public:
	explicit Error() = default;
	explicit Error(std::string && message);
	explicit Error(std::string && message, int code, std::string && description);

	int GetCode() const
	{
		return m_code;
	}

	explicit operator bool() const
	{
		return m_code != 0;
	}

	const std::string & GetMessage() const
	{
		return m_message;
	}

	const std::string & GetDescription() const
	{
		return m_description;
	}

	const std::string & GetWhat() const
	{
		return m_what;
	}

	const char *what() const noexcept override
	{
		return m_what.c_str();
	}
};

class SystemError : public Error
{
public:
	explicit SystemError() = default;
	explicit SystemError(std::string && message, int code = GetCurrentCode());

	static int GetCurrentCode();

	static std::string GetCodeDescription(int code);
};
