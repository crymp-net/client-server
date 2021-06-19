#pragma once

struct ICVar;

class GSMasterHook
{
	ICVar *m_pHostnameCVar = nullptr;

public:
	GSMasterHook();
	~GSMasterHook();

	ICVar *getHostnameCVar()
	{
		return m_pHostnameCVar;
	}
};
