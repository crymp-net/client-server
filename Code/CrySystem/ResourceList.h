#pragma once

#include <set>
#include <string>

#include "CryCommon/CrySystem/ICryPak.h"
#include "Library/StringTools.h"

class ResourceList : public IResourceList
{
	std::set<std::string, StringTools::PathComparator> m_paths;
	std::set<std::string, StringTools::PathComparator>::iterator m_it = m_paths.end();

	ResourceList() = default;

public:
	////////////////////////////////////////////////////////////////////////////////
	// IResourceList
	////////////////////////////////////////////////////////////////////////////////

	void Add(const char* resourceFile) override;
	void Clear() override;
	bool IsExist(const char* resourceFile) override;

	void Load(const char* resourceListFilename) override;

	const char* GetFirst() override;
	const char* GetNext() override;

	////////////////////////////////////////////////////////////////////////////////

	static _smart_ptr<IResourceList> Create();
};
