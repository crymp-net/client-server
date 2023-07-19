#pragma once

#include "CryCommon/CryAction/IViewSystem.h"
#include "CryCommon/CryMovie/IMovieSystem.h"

struct ISystem;

class ViewSystem : public IViewSystem, public IMovieUser
{
#ifdef BUILD_64BIT
	unsigned char m_data[0xa0 - 0x10] = {};
#else
	unsigned char m_data[0x70 - 0x8] = {};
#endif

public:
	explicit ViewSystem(ISystem* pSystem);

	////////////////////////////////////////////////////////////////////////////////
	// IViewSystem
	////////////////////////////////////////////////////////////////////////////////

	IView* CreateView() override;

	void SetActiveView(IView* pView) override;
	void SetActiveView(unsigned int viewId) override;

	IView* GetView(unsigned int viewId) override;
	IView* GetActiveView() override;

	unsigned int GetViewId(IView* pView) override;
	unsigned int GetActiveViewId() override;

	IView* GetViewByEntityId(unsigned int id, bool forceCreate) override;

	bool AddListener(IViewSystemListener* pListener) override;
	bool RemoveListener(IViewSystemListener* pListener) override;

	void Serialize(TSerialize ser) override;

	float GetDefaultZNear() override;

	void SetBlendParams(float blendPosSpeed, float blendRotSpeed, bool performBlendOut) override;
	void SetOverrideCameraRotation(bool enable, Quat rotation) override;

	bool IsPlayingCutScene() const override;

	////////////////////////////////////////////////////////////////////////////////
	// IMovieUser
	////////////////////////////////////////////////////////////////////////////////

	void SetActiveCamera(const SCameraParams& params) override;
	void BeginCutScene(IAnimSequence* pSeq, unsigned long flags, bool resetFX) override;
	void EndCutScene(IAnimSequence* pSeq) override;
	void SendGlobalEvent(const char* event) override;
	void PlaySubtitles(IAnimSequence* pSeq, ISound* pSound) override;

	////////////////////////////////////////////////////////////////////////////////
};
