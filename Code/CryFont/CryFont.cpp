#include <algorithm>

#include "CrySystem/CryLog.h"

#include "CryFont.h"
#include "Font.h"

CryFont::CryFont()
{
	FT_Error error = FT_Init_FreeType(&m_freetype);
	if (error)
	{
		CryLogErrorAlways("[CryFont] FreeType initialization failed with error %d", error);
	}
}

CryFont::~CryFont()
{
	FT_Done_FreeType(m_freetype);
}

void CryFont::Release()
{
}

IFFont* CryFont::NewFont(const char* name)
{
	if (const auto it = std::ranges::find(m_fonts, name, [](auto& f) { return f->name; }); it != m_fonts.end())
	{
		return it->get();
	}
	else
	{
		auto& font = m_fonts.emplace_back(std::make_unique<Font>());
		font->name = name;
		font->freetype = m_freetype;

		CryLogAlways("[CryFont] Created font %s", name);

		return font.get();
	}
}

IFFont* CryFont::GetFont(const char* name)
{
	if (const auto it = std::ranges::find(m_fonts, name, [](auto& f) { return f->name; }); it != m_fonts.end())
	{
		return it->get();
	}
	else
	{
		CryLogWarningAlways("[CryFont] Cannot find font %s", name);

		return nullptr;
	}
}

void CryFont::GetMemoryUsage(ICrySizer*)
{
}
