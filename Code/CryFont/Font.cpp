#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"

#include "Font.h"

Font::Font()
{
	this->ResetEffects();
}

Font::~Font()
{
	this->Free();
}

void Font::ResetEffects()
{
	this->effects.clear();
	this->effects.resize(1);
	this->effects[0].name = "default";
	this->currentEffectIndex = 0;
}

Font::Effect& Font::AddEffect(const std::string_view& name)
{
	if (name == "default")
	{
		return this->effects[0];
	}
	else
	{
		Effect& effect = this->effects.emplace_back();
		effect.name = name;
		return effect;
	}
}

std::string_view Font::GetDirectoryPath() const
{
	std::string_view path;

	if (const auto lastSlashPos = filename.find_last_of('/'); lastSlashPos != std::string::npos)
	{
		path = filename;
		path.remove_suffix(path.length() - (lastSlashPos + 1));  // keep trailing slash
	}

	return path;
}

void Font::Reset()
{
	// TODO
}

void Font::Release()
{
}

bool Font::Load(const char* filename, unsigned long width, unsigned long height, unsigned long flags)
{
	this->Free();

	CCryFile file;
	if (!file.Open(filename, "rb"))
	{
		CryLogErrorAlways("[CryFont] Failed to open %s", filename);
		return false;
	}

	const auto dataSize = file.GetLength();
	this->data = std::make_unique<FT_Byte[]>(dataSize);

	if (file.ReadRaw(this->data.get(), dataSize) != dataSize)
	{
		CryLogErrorAlways("[CryFont] Failed to read %s", filename);
		this->data = nullptr;
		return false;
	}

	FT_Error error = FT_New_Memory_Face(this->freetype, data.get(), dataSize, 0, &this->face);
	if (error)
	{
		CryLogErrorAlways("[CryFont] Failed to load %s (error code %d)", filename, error);
		this->data = nullptr;
		return false;
	}

	CryLogAlways("[CryFont] Loaded %s", filename);

	return true;
}

bool Font::Load(const char* filename)
{
	this->ResetEffects();
	this->Free();
	this->filename = filename;

	XmlNodeRef rootNode = gEnv->pSystem->LoadXmlFile(filename);
	if (!rootNode)
	{
		CryLogErrorAlways("[CryFont] Failed to load %s", filename);
		return false;
	}

	if (!this->ParseXML(rootNode))
	{
		CryLogErrorAlways("[CryFont] Failed to parse %s", filename);
		return false;
	}

	CryLogAlways("[CryFont] Loaded %s", filename);

	return true;
}

void Font::Free()
{
	FT_Done_Face(this->face);
	this->face = nullptr;
	this->data = nullptr;

	// TODO
}

void Font::SetEffect(const char* effect)
{
	// TODO
}

void Font::SetClippingRect(float x, float y, float w, float h)
{
	// TODO
}

void Font::EnableClipping(bool clipping)
{
	// TODO
}

void Font::SetColor(const ColorF& color, int pass)
{
	// TODO
}

void Font::UseRealPixels(bool realPixels)
{
	// TODO
}

bool Font::UsingRealPixels()
{
	// TODO
	return {};
}

void Font::SetSize(const vector2f& size)
{
	// TODO
}

vector2f& Font::GetSize()
{
	// TODO
	static vector2f value;
	return value;
}

float Font::GetCharWidth()
{
	// TODO
	return {};
}

float Font::GetCharHeight()
{
	// TODO
	return {};
}

void Font::SetSameSize(bool sameSize)
{
	// TODO
}

bool Font::GetSameSize()
{
	// TODO
	return {};
}

void Font::SetCharWidthScale(float scale)
{
	// TODO
}

float Font::GetCharWidthScale()
{
	// TODO
	return {};
}

void Font::DrawString(float x, float y, const char* text, bool multiline)
{
	const float z = 1;

	this->DrawString(x, y, z, text, multiline);
}

void Font::DrawString(float x, float y, float z, const char* text, bool multiline)
{
	// TODO
}

vector2f Font::GetTextSize(const char* text, bool multiline)
{
	// TODO
	return {};
}

void Font::DrawStringW(float x, float y, const wchar_t* text, bool multiline)
{
	// TODO
}

void Font::DrawStringW(float x, float y, float z, const wchar_t* text, bool multiline)
{
	// TODO
}

void Font::DrawWrappedStringW(float x, float y, float width, const wchar_t* text, bool multiline)
{
	// TODO
}

vector2f Font::GetTextSizeW(const wchar_t* text, bool multiline)
{
	// TODO
	return {};
}

vector2f Font::GetWrappedTextSizeW(const wchar_t* text, float width, bool multiline)
{
	// TODO
	return {};
}

int Font::GetTextLength(const char* text, bool multiline)
{
	// TODO
	return {};
}

int Font::GetTextLengthW(const wchar_t* text, bool multiline)
{
	// TODO
	return {};
}

void Font::GetMemoryUsage(ICrySizer*)
{
}

bool Font::ParseXML(XmlNodeRef& rootNode)
{
	XmlNodeRef fontNode = rootNode->findChild("font");
	if (!fontNode)
	{
		return false;
	}

	if (!this->ParseXML_FontNode(fontNode))
	{
		return false;
	}

	const int childCount = rootNode->getChildCount();
	for (int i = 0; i < childCount; i++)
	{
		XmlNodeRef childNode = rootNode->getChild(i);

		if (childNode->isTag("effect"))
		{
			this->ParseXML_EffectNode(childNode);
		}
	}

	return true;
}

bool Font::ParseXML_FontNode(XmlNodeRef& fontNode)
{
	XmlString fileName;
	if (!fontNode->getAttr("path", fileName))
	{
		return false;
	}

	std::string fontPath;
	fontPath.append(this->GetDirectoryPath());
	fontPath.append(fileName.c_str(), fileName.length());

	unsigned long width = 0;
	unsigned long height = 0;
	fontNode->getAttr("w", width);
	fontNode->getAttr("h", height);

	if (width == 0 || height == 0)
	{
		width = 512;
		height = 512;
	}

	XmlString smoothName;
	unsigned long smoothAmount = 0;
	fontNode->getAttr("smooth", smoothName);
	fontNode->getAttr("smooth_amount", smoothAmount);

	unsigned long smooth = TTFFLAG_SMOOTH_NONE;

	if (smoothName == "blur")
	{
		smooth = TTFFLAG_SMOOTH_BLUR;
	}
	else if (smoothName == "supersample")
	{
		smooth = TTFFLAG_SMOOTH_SUPERSAMPLE;
	}

	const unsigned long flags = TTFFLAG_CREATE(smooth, smoothAmount);

	return this->Load(fontPath.c_str(), width, height, flags);
}

bool Font::ParseXML_EffectNode(XmlNodeRef& effectNode)
{
	XmlString name;
	if (!effectNode->getAttr("name", name))
	{
		return false;
	}

	Effect& effect = this->AddEffect(std::string_view(name.c_str(), name.length()));

	const int childCount = effectNode->getChildCount();
	for (int i = 0; i < childCount; i++)
	{
		XmlNodeRef childNode = effectNode->getChild(i);

		if (childNode->isTag("pass"))
		{
			this->ParseXML_EffectPassNode(childNode, effect.passes.emplace_back());
		}
	}

	return true;
}

bool Font::ParseXML_EffectPassNode(XmlNodeRef& passNode, Effect::Pass& pass)
{
	const int childCount = passNode->getChildCount();
	for (int i = 0; i < childCount; i++)
	{
		XmlNodeRef childNode = passNode->getChild(i);

		if (childNode->isTag("color"))
		{
			childNode->getAttr("r", pass.color.r);
			childNode->getAttr("g", pass.color.g);
			childNode->getAttr("b", pass.color.b);
			childNode->getAttr("a", pass.color.a);
		}
		else if (childNode->isTag("size"))
		{
			float scale = 1;
			childNode->getAttr("scale", scale);
			pass.size.x = scale;
			pass.size.y = scale;

			childNode->getAttr("x", pass.size.x);
			childNode->getAttr("y", pass.size.y);
		}
		else if (childNode->isTag("pos") || childNode->isTag("offset"))
		{
			childNode->getAttr("x", pass.pos.x);
			childNode->getAttr("y", pass.pos.y);
		}
		else if (childNode->isTag("blend") || childNode->isTag("blending"))
		{
			this->ParseXML_EffectPassBlendNode(childNode, pass);
		}
	}

	return true;
}

bool Font::ParseXML_EffectPassBlendNode(XmlNodeRef& blendNode, Effect::Pass& pass)
{
	constexpr struct { const char* name; int value; } BLEND_SRC_TABLE[] = {
		{ "zero", 0x1 },
		{ "one", 0x2 },
		{ "srccolor", 0x30 },
		{ "src_color", 0x30 },
		{ "invsrccolor", 0x40 },
		{ "inv_src_color", 0x40 },
		{ "dstcolor", 0x3 },
		{ "dst_color", 0x3 },
		{ "invdstcolor", 0x4 },
		{ "inv_dst_color", 0x4 },
		{ "srcalpha", 0x5 },
		{ "src_alpha", 0x5 },
		{ "invsrcalpha", 0x6 },
		{ "inv_src_alpha", 0x6 },
		{ "dstalpha", 0x7 },
		{ "dst_alpha", 0x7 },
		{ "invdstalpha", 0x8 },
		{ "inv_dst_alpha", 0x8 },
	};

	constexpr struct { const char* name; int value; } BLEND_DST_TABLE[] = {
		{ "zero", 0x10 },
		{ "one", 0x20 },
		{ "srccolor", 0x30 },
		{ "src_color", 0x30 },
		{ "invsrccolor", 0x40 },
		{ "inv_src_color", 0x40 },
		{ "dstcolor", 0x3 },
		{ "dst_color", 0x3 },
		{ "invdstcolor", 0x4 },
		{ "inv_dst_color", 0x4 },
		{ "srcalpha", 0x50 },
		{ "src_alpha", 0x50 },
		{ "invsrcalpha", 0x60 },
		{ "inv_src_alpha", 0x60 },
		{ "dstalpha", 0x70 },
		{ "dst_alpha", 0x70 },
		{ "invdstalpha", 0x80 },
		{ "inv_dst_alpha", 0x80 },
	};

	const auto findBlendValue = [](const auto& table, const XmlString& name) -> int
	{
		for (const auto& entry : table)
		{
			if (name == entry.name)
			{
				return entry.value;
			}
		}

		return 0x2;
	};

	XmlString value;

	if (blendNode->getAttr("src", value))
	{
		pass.blendSrc = findBlendValue(BLEND_SRC_TABLE, value);
	}

	if (blendNode->getAttr("dst", value))
	{
		pass.blendDst = findBlendValue(BLEND_DST_TABLE, value);
	}

	if (blendNode->getAttr("type", value))
	{
		if (value == "modulate")
		{
			pass.blendSrc = 0x5;
			pass.blendDst = 0x60;
		}
		else if (value == "additive")
		{
			pass.blendSrc = 0x5;
			pass.blendDst = 0x20;
		}
	}

	return true;
}
