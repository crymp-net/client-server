#pragma once
#include <map>
#include <set>
#include <list>

struct IActor;

class CHealthManager {
	double m_time = 0.0;
public:
	CHealthManager();
	~CHealthManager();
	bool IsActive();
	int GetHealth(IActor* pActor);
	void Reset();
};