#pragma once

struct ICVar;

class GSMasterHook
{
	ICVar *m_pHostnameCVar = nullptr;

public:
	GSMasterHook();
	~GSMasterHook();

	ICVar *GetHostnameCVar()
	{
		return m_pHostnameCVar;
	}
};
