#pragma once

struct ICVar;

class GSMasterHook
{
	ICVar *m_pHostnameCVar;

public:
	GSMasterHook();
	~GSMasterHook();

	ICVar *getHostnameCVar()
	{
		return m_pHostnameCVar;
	}
};
