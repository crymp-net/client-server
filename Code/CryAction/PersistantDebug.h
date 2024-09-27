#pragma once

#include "CryCommon/CryAction/IGameFramework.h"

class PersistantDebug : public IPersistantDebug
{
	void* reserved[11 - 1] = {};

public:
	PersistantDebug();

	////////////////////////////////////////////////////////////////////////////////
	// IPersistantDebug
	////////////////////////////////////////////////////////////////////////////////

	void Begin(const char* name, bool clear) override;
	void AddSphere(const Vec3& pos, float radius, ColorF clr, float timeout) override;
	void AddDirection(const Vec3& pos, float radius, const Vec3& dir, ColorF clr, float timeout) override;
	void AddLine(const Vec3& pos1, const Vec3& pos2, ColorF clr, float timeout) override;
	void AddPlanarDisc(const Vec3& pos, float innerRadius, float outerRadius, ColorF clr, float timeout) override;
	void Add2DText (const char* text, float size, ColorF clr, float timeout) override;
	void AddText (float x, float y, float size, ColorF clr, float timeout, const char* fmt, ...) override;
	void Add2DLine(float x1, float y1, float x2, float y2, ColorF clr, float timeout) override;
	void AddQuat(const Vec3& pos, const Quat& q, float r, ColorF clr, float timeout) override;

	////////////////////////////////////////////////////////////////////////////////
};
