#pragma once

struct ICVar;

class GSMasterHook
{
public:
	GSMasterHook();
	~GSMasterHook();

	ICVar *GetHostnameCVar();
};
