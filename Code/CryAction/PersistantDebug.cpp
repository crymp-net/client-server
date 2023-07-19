#include <cstdint>

#include "PersistantDebug.h"

PersistantDebug::PersistantDebug()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30784670;
#else
	std::uintptr_t ctor = 0x306ba8d0;
#endif

	(this->*reinterpret_cast<void(PersistantDebug::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IPersistantDebug
////////////////////////////////////////////////////////////////////////////////

void PersistantDebug::Begin(const char* name, bool clear)
{
}

void PersistantDebug::AddSphere(const Vec3& pos, float radius, ColorF clr, float timeout)
{
}

void PersistantDebug::AddDirection(const Vec3& pos, float radius, const Vec3& dir, ColorF clr, float timeout)
{
}

void PersistantDebug::AddLine(const Vec3& pos1, const Vec3& pos2, ColorF clr, float timeout)
{
}

void PersistantDebug::AddPlanarDisc(const Vec3& pos, float innerRadius, float outerRadius, ColorF clr, float timeout)
{
}

void PersistantDebug::Add2DText (const char* text, float size, ColorF clr, float timeout)
{
}

void PersistantDebug::AddText (float x, float y, float size, ColorF clr, float timeout, const char* fmt, ...)
{
}

void PersistantDebug::Add2DLine(float x1, float y1, float x2, float y2, ColorF clr, float timeout)
{
}

void PersistantDebug::AddQuat(const Vec3& pos, const Quat& q, float r, ColorF clr, float timeout)
{
}

////////////////////////////////////////////////////////////////////////////////
