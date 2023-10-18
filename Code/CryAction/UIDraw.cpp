#include <cstdint>

#include "UIDraw.h"

UIDraw::UIDraw()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307801e0;
#else
	std::uintptr_t ctor = 0x306b85b0;
#endif

	(this->*reinterpret_cast<void(UIDraw::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IUIDraw
////////////////////////////////////////////////////////////////////////////////

void UIDraw::Release()
{
}

void UIDraw::PreRender()
{
}

void UIDraw::PostRender()
{
}

unsigned int UIDraw::GetColorARGB(
	std::uint8_t alpha,
	std::uint8_t red,
	std::uint8_t green,
	std::uint8_t blue)
{
	return 0;
}

int UIDraw::CreateTexture(const char *name)
{
	return 0;
}

void UIDraw::GetTextureSize(int textureID, float& sizeX, float& sizeY)
{
}

void UIDraw::DrawQuad(
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
	bool use169)
{
}

void UIDraw::DrawImage(
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
	float t1)
{
}

void UIDraw::DrawImageCentered(
	int textureID,
	float x,
	float y,
	float sizeX,
	float sizeY,
	float angleInDegrees,
	float red,
	float green,
	float blue,
	float alpha)
{
}

void UIDraw::DrawText(
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
	EUIDRAWVERTICAL vertical)
{
}

void UIDraw::GetTextDim(
	IFFont* pFont,
	float* width,
	float* height,
	float sizeX,
	float sizeY,
	const char *text)
{
}

void UIDraw::DrawTextW(
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
	EUIDRAWVERTICAL vertical)
{
}

void UIDraw::DrawWrappedTextW(
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
	EUIDRAWVERTICAL vertical)
{
}

void UIDraw::GetTextDimW(
	IFFont* pFont,
	float* width,
	float* height,
	float sizeX,
	float sizeY,
	const wchar_t* text)
{
}

void UIDraw::GetWrappedTextDimW(
	IFFont* pFont,
	float* width,
	float* height,
	float maxWidth,
	float sizeX,
	float sizeY,
	const wchar_t* text)
{
}

////////////////////////////////////////////////////////////////////////////////
