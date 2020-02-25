/**
 * @file
 * @brief Implementation of custom r_DisplayInfo.
 */

#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "CryCommon/I3DEngine.h"
#include "CryCommon/IRenderer.h"
#include "CryCommon/ITimer.h"
#include "Library/Hook/VTable.h"
#include "Library/StringBuffer.h"

#include "DisplayInfo.h"

struct Dummy3DEngine
{
	void DisplayInfo(float & textPosX, float & textPosY, float & textStepY, bool isEnhanced);
};

using TDisplayInfo = void (Dummy3DEngine::*)(float &, float &, float &, bool);

static ICVar *g_pNewDisplayInfoCVar;
static void *g_pOldDisplayInfo;

static inline void Blend(float & stat, float statCurrent, float blendCurrent)
{
	stat = stat * (1.0 - blendCurrent) + statCurrent * blendCurrent;
}

void Dummy3DEngine::DisplayInfo(float & textPosX, float & textPosY, float & textStepY, bool isEnhanced)
{
	if (g_pNewDisplayInfoCVar->GetIVal() == 0)
	{
		(this->*reinterpret_cast<TDisplayInfo&>(g_pOldDisplayInfo))(textPosX, textPosY, textStepY, isEnhanced);
		return;
	}

	ITimer *pTimer = gEnv->pTimer;
	IRenderer *pRenderer = gEnv->pRenderer;

	StringBuffer<1024> buffer;

	float blendTime = pTimer->GetCurrTime();
	int blendMode = 0;
	float blendCurrent = pTimer->GetProfileFrameBlending(&blendTime, &blendMode);

	static float currentFPS;
	static float currentFrameTime;
	Blend(currentFPS, pTimer->GetFrameRate(), blendCurrent);
	Blend(currentFrameTime, pTimer->GetRealFrameTime() * 1000.0, blendCurrent);

	buffer.append_f("FPS %.1f - %.1fms", currentFPS, currentFrameTime);

	SDrawTextInfo textInfo;
	textInfo.xscale = 1.5;
	textInfo.yscale = 1.5;

	pRenderer->Draw2dText(10, 10, buffer.get(), textInfo);

	// TODO
}

bool DisplayInfo::Init()
{
	if (g_pOldDisplayInfo)
	{
		return true;
	}

	I3DEngine *p3DEngine = gEnv->p3DEngine;
	if (!p3DEngine)
	{
		return false;
	}

	g_pNewDisplayInfoCVar = gEnv->pConsole->RegisterInt("cl_newDisplayInfo", 0, VF_RESTRICTEDMODE | VF_NOT_NET_SYNCED);

	void *p3DEngineVTable = *reinterpret_cast<void**>(p3DEngine);
	TDisplayInfo pNewDisplayInfo = &Dummy3DEngine::DisplayInfo;

	return VTableHook(p3DEngineVTable, 99, reinterpret_cast<void*&>(pNewDisplayInfo), &g_pOldDisplayInfo);
}
