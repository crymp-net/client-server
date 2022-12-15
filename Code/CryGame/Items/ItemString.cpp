#include "ItemString.h"

std::string_view ItemString::NameTable::GetEntry(const std::string_view& name)
{
	auto it = m_table.find<std::string_view>(name);
	if (it == m_table.end())
	{
		std::tie(it, std::ignore) = m_table.emplace(name);
	}

	return std::string_view(*it);
}

void ItemString::NameTable::DumpToLog()
{
	CryLogAlways("NameTable: %zu entries", m_table.size());

	for (const std::string& entry : m_table)
	{
		CryLogAlways("'%s'", entry.c_str());
	}
}
