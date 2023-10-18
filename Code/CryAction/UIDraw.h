#pragma once

#include "CryCommon/CryAction/IUIDraw.h"

class UIDraw : public IUIDraw
{
	void* m_reserved[8 - 1] = {};

public:
	UIDraw();

	////////////////////////////////////////////////////////////////////////////////
	// IUIDraw
	////////////////////////////////////////////////////////////////////////////////

	void Release() override;

	void PreRender() override;
	void PostRender() override;

	unsigned int GetColorARGB(
		std::uint8_t alpha,
		std::uint8_t red,
		std::uint8_t green,
		std::uint8_t blue) override;

	int CreateTexture(const char *name) override;

	void GetTextureSize(int textureID, float& sizeX, float& sizeY) override;

	void DrawQuad(
		float x,
		float y,
		float sizeX,
		float sizeY,
		unsigned int diffuse,
		unsigned int diffuseTL,
		unsigned int diffuseTR,
		unsigned int diffuseDL,
		unsigned int diffuseDR,
		int textureID,
		float fUTexCoordsTL,
		float fVTexCoordsTL,
		float fUTexCoordsTR,
		float fVTexCoordsTR,
		float fUTexCoordsDL,
		float fVTexCoordsDL,
		float fUTexCoordsDR,
		float fVTexCoordsDR,
		bool use169) override;

	void DrawImage(
		int textureID,
		float x,
		float y,
		float sizeX,
		float sizeY,
		float angleInDegrees,
		float red,
		float green,
		float blue,
		float alpha,
		float s0,
		float t0,
		float s1,
		float t1) override;

	void DrawImageCentered(
		int textureID,
		float x,
		float y,
		float sizeX,
		float sizeY,
		float angleInDegrees,
		float red,
		float green,
		float blue,
		float alpha) override;

	void DrawText(
		IFFont* pFont,
		float fX,
		float fY,
		float fSizeX,
		float fSizeY,
		const char *strText,
		float fAlpha,
		float fRed,
		float fGreen,
		float fBlue,
		EUIDRAWHORIZONTAL horizontalDocking,
		EUIDRAWVERTICAL verticalDocking,
		EUIDRAWHORIZONTAL horizontal,
		EUIDRAWVERTICAL vertical) override;

	void GetTextDim(
		IFFont* pFont,
		float* width,
		float* height,
		float sizeX,
		float sizeY,
		const char *text) override;

	void DrawTextW(
		IFFont* pFont,
		float x,
		float y,
		float sizeX,
		float sizeY,
		const wchar_t* text,
		float alpha,
		float red,
		float green,
		float blue,
		EUIDRAWHORIZONTAL horizontalDocking,
		EUIDRAWVERTICAL verticalDocking,
		EUIDRAWHORIZONTAL horizontal,
		EUIDRAWVERTICAL vertical) override;

	void DrawWrappedTextW(
		IFFont* pFont,
		float x,
		float y,
		float maxWidth,
		float sizeX,
		float sizeY,
		const wchar_t* text,
		float alpha,
		float red,
		float green,
		float blue,
		EUIDRAWHORIZONTAL horizontalDocking,
		EUIDRAWVERTICAL verticalDocking,
		EUIDRAWHORIZONTAL horizontal,
		EUIDRAWVERTICAL vertical) override;

	void GetTextDimW(
		IFFont* pFont,
		float* width,
		float* height,
		float sizeX,
		float sizeY,
		const wchar_t* text) override;

	void GetWrappedTextDimW(
		IFFont* pFont,
		float* width,
		float* height,
		float maxWidth,
		float sizeX,
		float sizeY,
		const wchar_t* text) override;

	////////////////////////////////////////////////////////////////////////////////
};
