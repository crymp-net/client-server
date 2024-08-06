#pragma once

#include "CryCommon/CryAction/ISubtitleManager.h"

class SubtitleManager : public ISubtitleManager
{
	void* m_reserved[5 - 1] = {};

public:
	SubtitleManager();
	virtual ~SubtitleManager();

	////////////////////////////////////////////////////////////////////////////////
	// ISubtitleManager
	////////////////////////////////////////////////////////////////////////////////

	void SetHandler(ISubtitleHandler* pHandler) override;
	ISubtitleHandler* GetHandler() override;

	void SetEnabled(bool enabled) override;

	void SetAutoMode(bool on) override;

	void ShowSubtitle(ISound* pSound, bool show) override;
	void ShowSubtitle(const char* subtitleLabel, bool show) override;

	////////////////////////////////////////////////////////////////////////////////
};
