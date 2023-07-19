#pragma once

#include "CryCommon/CryAction/IActionMapManager.h"
#include "CryCommon/CryInput/IInput.h"

class ActionMapManager : public IActionMapManager, public IInputEventListener
{
#ifdef BUILD_64BIT
	unsigned char m_data[0xb0 - 0x10] = {};
#else
	unsigned char m_data[0x5c - 0x8] = {};
#endif

public:
	ActionMapManager(IInput* pInput);
	virtual ~ActionMapManager();

	////////////////////////////////////////////////////////////////////////////////
	// IActionMapManager
	////////////////////////////////////////////////////////////////////////////////

	void Update() override;
	void Reset() override;
	void Clear() override;

	void LoadFromXML(const XmlNodeRef& node, bool checkVersion) override;
	void SaveToXML(const XmlNodeRef& node) override;

	IActionMap* CreateActionMap(const char* name) override;
	IActionMap* GetActionMap(const char* name) override;
	IActionFilter* CreateActionFilter(const char* name, EActionFilterType type) override;
	IActionFilter* GetActionFilter(const char* name) override;
	IActionMapIteratorPtr CreateActionMapIterator() override;
	IActionFilterIteratorPtr CreateActionFilterIterator() override;

	void Enable(bool enable) override;
	void EnableActionMap(const char* name, bool enable) override;
	void EnableFilter(const char* name, bool enable) override;
	bool IsFilterEnabled(const char* name) override;

	////////////////////////////////////////////////////////////////////////////////
	// IInputEventListener
	////////////////////////////////////////////////////////////////////////////////

	bool OnInputEvent(const SInputEvent& event) override;
	bool OnInputEventUI(const SInputEvent& event) override;

	////////////////////////////////////////////////////////////////////////////////
};
