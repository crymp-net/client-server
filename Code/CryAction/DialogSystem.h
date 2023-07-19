#pragma once

#include "CryCommon/CryAction/IDialogSystem.h"

class DialogSystem : public IDialogSystem
{
	void* m_reserved[23 - 1] = {};

public:
	DialogSystem();
	virtual ~DialogSystem();

	virtual bool Init();

	////////////////////////////////////////////////////////////////////////////////
	// IDialogSystem
	////////////////////////////////////////////////////////////////////////////////

	void Update(const float dt) override;
	void Reset() override;
	bool ReloadScripts() override;
	IDialogScriptIteratorPtr CreateScriptIterator() override;

	////////////////////////////////////////////////////////////////////////////////
};
