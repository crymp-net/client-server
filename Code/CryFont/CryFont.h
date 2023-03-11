#pragma once

#include <memory>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "CryCommon/CryFont/IFont.h"

struct Font;

class CryFont final : public ICryFont
{
	FT_Library m_freetype = nullptr;

	std::vector<std::unique_ptr<Font>> m_fonts;

public:
	CryFont();
	~CryFont();

	////////////////////////////////////////////////////////////////////////////////
	// ICryFont
	////////////////////////////////////////////////////////////////////////////////

	void Release() override;

	IFFont* NewFont(const char* name) override;
	IFFont* GetFont(const char* name) override;

	void GetMemoryUsage(ICrySizer*) override;

	////////////////////////////////////////////////////////////////////////////////
};
