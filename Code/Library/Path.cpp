#include <cctype>
#include <cstring>
#include <algorithm>

#include "Path.h"

static bool IsSeparator(char ch)
{
	return ch == '/' || ch == '\\' || ch == '\0';
}

bool Path::assign(const char *path)
{
	clear();

	if (!path)
	{
		return false;
	}

	m_path.resize(1);

	for (size_t i = 0; path[i] != '\0'; i++)
	{
		char ch = path[i];

		switch (ch)
		{
			case '/':
			case '\\':
			{
				if (m_path.back().empty())
				{
					if (i == 0)
					{
						m_diskLetter = '/';
					}
				}
				else
				{
					m_path.push_back(std::string());
				}

				break;
			}
			case '.':
			{
				char prev = (i > 0) ? path[i-1] : '\0';
				char next = path[i+1];

				if (IsSeparator(prev) && IsSeparator(next))  // "/./"
				{
					// do nothing
				}
				else if (IsSeparator(prev) && next == '.' && IsSeparator(path[i+2]))  // "/../"
				{
					if (getSize() > 1)
					{
						m_path.pop_back();
						m_path.back().clear();
					}
					else if (isRelative())
					{
						m_parentCount++;
					}

					i++;
				}
				else
				{
					m_path.back() += ch;
				}

				break;
			}
			case ':':
			{
				if (getSize() == 1 && isRelative() && IsSeparator(path[i+1])
				 && m_path.back().length() == 1 && std::isalnum(m_path.back()[0]))
				{
					m_diskLetter = m_path.back()[0];
					m_path.back().clear();
				}
				else
				{
					m_path.back() += ch;
				}

				break;
			}
			default:
			{
				if (ch < 32 || ch == 127)
				{
					// replace control characters with '_'
					m_path.back() += '_';
				}
				else
				{
					m_path.back() += ch;
				}

				break;
			}
		}
	}

	if (m_path.back().empty())
	{
		m_path.pop_back();
	}

	return true;
}

bool Path::append(const Path & other)
{
	for (unsigned int i = 0; i < other.getParentCount(); i++)
	{
		if (m_path.empty())
		{
			if (isRelative())
			{
				m_parentCount++;
			}
			else
			{
				break;
			}
		}
		else
		{
			m_path.pop_back();
		}
	}

	m_path.reserve(getSize() + other.getSize());

	for (size_t i = 0; i < other.getSize(); i++)
	{
		m_path.push_back(other[i]);
	}

	return true;
}

bool Path::makeAbsolute(const Path & base)
{
	if (!base.isAbsolute())
	{
		return false;
	}

	const size_t baseSize = (m_parentCount < base.getSize()) ? base.getSize() - m_parentCount : 0;

	m_path.insert(m_path.begin(), baseSize, std::string());

	for (size_t i = 0; i < baseSize; i++)
	{
		m_path[i] = base.m_path[i + m_parentCount];
	}

	m_parentCount = 0;
	m_diskLetter = base.getDiskLetter();

	return true;
}

int Path::compare(const Path & other) const
{
	if (isRelative() != other.isRelative())
	{
		// relative paths are always before absolute paths
		return isRelative() ? -1 : 1;
	}

	if (isAbsolute())
	{
		if (m_diskLetter != other.getDiskLetter())
		{
			return (m_diskLetter < other.getDiskLetter()) ? -1 : 1;
		}
	}
	else
	{
		if (m_parentCount != other.getParentCount())
		{
			return (m_parentCount < other.getParentCount()) ? -1 : 1;
		}
	}

	const size_t size = std::min(getSize(), other.getSize());

	for (size_t i = 0; i < size; i++)
	{
		int result = m_path[i].compare(other[i]);
		if (result)
		{
			return result;
		}
	}

	if (getSize() != other.getSize())
	{
		return (getSize() < other.getSize()) ? -1 : 1;
	}

	return 0;
}

std::string Path::toString(EPathStyle style) const
{
	std::string result;

	if (isAbsolute())
	{
		if (m_diskLetter == '/')
		{
			if (style == WINDOWS)
			{
				result += '\\';
				result += '\\';
			}
			else
			{
				result += '/';
			}
		}
		else
		{
			if (style == WINDOWS)
			{
				result += m_diskLetter;
				result += ':';
				result += '\\';
			}
		}
	}
	else
	{
		const char *parent = (style == WINDOWS) ? "..\\" : "../";

		for (unsigned int i = 0; i < m_parentCount; i++)
		{
			result += parent;
		}
	}

	if (!m_path.empty())
	{
		char separator = (style == WINDOWS) ? '\\' : '/';

		result += m_path[0];

		for (size_t i = 1; i < getSize(); i++)
		{
			result += separator;
			result += m_path[i];
		}
	}

	return result;
}
