#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "CryCommon/CryFont/IFont.h"

class XmlNodeRef;

struct Font final : public IFFont
{
	std::string name;
	std::string filename;

	FT_Library freetype = nullptr;
	FT_Face face = nullptr;

	std::unique_ptr<FT_Byte[]> data;

	struct Effect
	{
		struct Pass
		{
			ColorF color = Col_White;
			Vec2 size = { 1, 1 };
			Vec2 pos = { 0, 0 };
			int blendSrc = 0x5;
			int blendDst = 0x60;
		};

		std::string name;
		std::vector<Pass> passes;
	};

	std::vector<Effect> effects;
	unsigned int currentEffectIndex = 0;

	Font();
	~Font();

	void ResetEffects();
	Effect& AddEffect(const std::string_view& name);

	std::string_view GetDirectoryPath() const;

	////////////////////////////////////////////////////////////////////////////////
	// IFFont
	////////////////////////////////////////////////////////////////////////////////

	void Reset() override;

	void Release() override;

	bool Load(const char* filename, unsigned long width, unsigned long height, unsigned long flags) override;
	bool Load(const char* filename) override;

	void Free() override;

	void SetEffect(const char* effect) override;

	void SetClippingRect(float x, float y, float w, float h) override;

	void EnableClipping(bool clipping) override;

	void SetColor(const ColorF& color, int pass = 0) override;

	void UseRealPixels(bool realPixels) override;
	bool UsingRealPixels() override;

	void SetSize(const vector2f& size) override;
	vector2f& GetSize() override;

	float GetCharWidth() override;
	float GetCharHeight() override;

	void SetSameSize(bool sameSize) override;
	bool GetSameSize() override;

	void SetCharWidthScale(float scale) override;
	float GetCharWidthScale() override;

	void DrawString(float x, float y, const char* text, bool multiline = true) override;
	void DrawString(float x, float y, float z, const char* text, bool multiline = true) override;

	vector2f GetTextSize(const char* text, bool multiline = true) override;

	void DrawStringW(float x, float y, const wchar_t* text, bool multiline = true) override;
	void DrawStringW(float x, float y, float z, const wchar_t* text, bool multiline = true) override;
	void DrawWrappedStringW(float x, float y, float width, const wchar_t* text, bool multiline = true) override;

	vector2f GetTextSizeW(const wchar_t* text, bool multiline = true) override;
	vector2f GetWrappedTextSizeW(const wchar_t* text, float width, bool multiline = true) override;

	int GetTextLength(const char* text, bool multiline = true) override;
	int GetTextLengthW(const wchar_t* text, bool multiline = true) override;

	void GetMemoryUsage(ICrySizer*) override;

	////////////////////////////////////////////////////////////////////////////////

private:
	bool ParseXML(XmlNodeRef& rootNode);
	bool ParseXML_FontNode(XmlNodeRef& fontNode);
	bool ParseXML_EffectNode(XmlNodeRef& effectNode);
	bool ParseXML_EffectPassNode(XmlNodeRef& passNode, Effect::Pass& pass);
	bool ParseXML_EffectPassBlendNode(XmlNodeRef& blendNode, Effect::Pass& pass);
};
