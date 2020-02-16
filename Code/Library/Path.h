#pragma once

#include <stddef.h>
#include <string>
#include <vector>

class Path
{
public:
	enum EPathStyle
	{
		UNIX, WINDOWS
	};

private:
	std::vector<std::string> m_path;
	unsigned int m_parentCount;  // number of ".." before the path
	char m_diskLetter;  // disk letter or '/' for absolute paths or zero for relative paths

public:
	Path()
	: m_path(),
	  m_parentCount(),
	  m_diskLetter()
	{
	}

	Path(const char *path)
	: m_path(),
	  m_parentCount(),
	  m_diskLetter()
	{
		assign(path);
	}

	Path(const std::string & path)
	: m_path(),
	  m_parentCount(),
	  m_diskLetter()
	{
		assign(path);
	}

	void clear()
	{
		m_path.clear();
		m_parentCount = 0;
		m_diskLetter = '\0';
	}

	void pop()
	{
		if (!m_path.empty())
		{
			m_path.pop_back();
		}
	}

	bool assign(const char *path);

	bool assign(const std::string & path)
	{
		return assign(path.c_str());
	}

	bool append(const Path & other);

	bool append(const std::string & other)
	{
		return append(Path(other));
	}

	bool append(const char *other)
	{
		return append(Path(other));
	}

	bool makeAbsolute(const Path & base);

	bool isEmpty() const
	{
		return m_path.empty() && m_parentCount == 0 && m_diskLetter == '\0';
	}

	bool isRelative() const
	{
		return !isAbsolute();
	}

	bool isAbsolute() const
	{
		return m_diskLetter != '\0';
	}

	char getDiskLetter() const
	{
		return m_diskLetter;
	}

	unsigned int getParentCount() const
	{
		return m_parentCount;
	}

	size_t getSize() const
	{
		return m_path.size();
	}

	const std::string & operator[](size_t index) const
	{
		return m_path[index];
	}

	const std::string & getFirst() const
	{
		return m_path.front();  // empty container = undefined behavior, always check with getSize
	}

	const std::string & getLast() const
	{
		return m_path.back();  // empty container = undefined behavior, always check with getSize
	}

	const std::vector<std::string> & get() const
	{
		return m_path;
	}

	int compare(const Path & other) const;

	std::string toString(EPathStyle style = UNIX) const;
};

inline bool operator==(const Path & a, const Path & b)
{
	return a.compare(b) == 0;
}

inline bool operator!=(const Path & a, const Path & b)
{
	return a.compare(b) != 0;
}

inline bool operator<(const Path & a, const Path & b)
{
	return a.compare(b) < 0;
}

inline bool operator>(const Path & a, const Path & b)
{
	return a.compare(b) > 0;
}

inline bool operator<=(const Path & a, const Path & b)
{
	return a.compare(b) <= 0;
}

inline bool operator>=(const Path & a, const Path & b)
{
	return a.compare(b) >= 0;
}

inline Path & operator+=(Path & a, const Path & b)
{
	a.append(b);
	return a;
}

inline Path & operator+=(Path & a, const std::string & b)
{
	a.append(b);
	return a;
}

inline Path & operator+=(Path & a, const char *b)
{
	a.append(b);
	return a;
}

inline Path operator+(const Path & a, const Path & b)
{
	Path c = a;
	c.append(b);
	return c;
}

inline Path operator+(const Path & a, const std::string & b)
{
	Path c = a;
	c.append(b);
	return c;
}

inline Path operator+(const Path & a, const char *b)
{
	Path c = a;
	c.append(b);
	return c;
}
