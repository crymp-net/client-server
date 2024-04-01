#include "ResourceList.h"

////////////////////////////////////////////////////////////////////////////////
// IResourceList
////////////////////////////////////////////////////////////////////////////////

void ResourceList::Add(const char* resourceFile)
{
	const std::string_view path = StringTools::SafeView(resourceFile);

	if (!path.empty())
	{
		m_paths.emplace(path);
	}
}

void ResourceList::Clear()
{
	m_paths.clear();
	m_it = m_paths.end();
}

bool ResourceList::IsExist(const char* resourceFile)
{
	return m_paths.contains(StringTools::SafeView(resourceFile));
}

void ResourceList::Load(const char* resourceListFilename)
{
	std::string content;
	{
		CCryFile file;
		if (file.Open(resourceListFilename, "rb"))
		{
			content.resize(file.GetLength());
			content.resize(file.ReadRaw(content.data(), content.size()));
		}
	}

	std::string::size_type pos = 0;
	while (pos < content.length())
	{
		std::string::size_type newlinePos = content.find('\n', pos);
		if (newlinePos == std::string::npos)
		{
			newlinePos = content.length();
		}

		std::string_view line(content.data() + pos, newlinePos - pos);

		if (!line.empty() && line.back() == '\r')
		{
			line.remove_suffix(1);
		}

		if (!line.empty() && !StringTools::StartsWithNoCase(line, "editor/"))
		{
			m_paths.emplace(line);
		}

		pos = newlinePos + 1;
	}
}

const char* ResourceList::GetFirst()
{
	m_it = m_paths.begin();

	if (m_it == m_paths.end())
	{
		return nullptr;
	}

	return m_it->c_str();
}

const char* ResourceList::GetNext()
{
	if (m_it == m_paths.end())
	{
		return nullptr;
	}

	++m_it;

	if (m_it == m_paths.end())
	{
		return nullptr;
	}

	return m_it->c_str();
}

////////////////////////////////////////////////////////////////////////////////

_smart_ptr<IResourceList> ResourceList::Create()
{
	return _smart_ptr<IResourceList>(new ResourceList);
}
