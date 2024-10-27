#pragma once

#include "CryCommon/CryAction/IDebugHistory.h"

class DebugHistoryManager : public IDebugHistoryManager
{
	void* m_reserved[6] = {};

public:
	DebugHistoryManager();

	////////////////////////////////////////////////////////////////////////////////
	// IDebugHistoryManager
	////////////////////////////////////////////////////////////////////////////////

	IDebugHistory* CreateHistory(const char* id, const char* name) override;
	void RemoveHistory(const char* name) override;
	IDebugHistory* GetHistory(const char* name) override;
	void Clear() override;
	void GetMemoryStatistics(ICrySizer*) override;
	void Release() override;

	void LayoutHelper(const char* id, const char* name, bool visible, float minout, float maxout,
		float minin, float maxin, float x, float y, float w, float h) override;

	////////////////////////////////////////////////////////////////////////////////
};
