#pragma once

struct ILevelInfo;

class EngineCache
{
	bool m_recursing = false;
	bool m_isCaching = false;
	int cl_engineCacheLevel = 0;
	
	enum ECacheLevel
	{
		DISABLED,
		MINIMUM,
		RECOMMENDED,
		HIGH,
		LUDICROUS
	};

	ECacheLevel m_cacheStatus = ECacheLevel::DISABLED;

public:
	EngineCache();
	~EngineCache();

	int ScanFolder(const char* folderName);
	bool Cache(string folder, string file);
	void OnLoadingStart(ILevelInfo* pLevel);
	void OnLoadingProgress(ILevelInfo* pLevel, int progressAmount);
	void Start();
	ECacheLevel GetStatus()
	{
		return m_cacheStatus;
	}

};
