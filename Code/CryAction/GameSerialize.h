#pragma once

#include "CryCommon/CryAction/IGameFramework.h"

class GameFramework;

class GameSerialize
{
	void* m_reserved[15] = {};

public:
	GameSerialize();

	void RegisterFactories(IGameFramework* pGameFramework);

	void RegisterLoadGameFactory(const char* name, ILoadGame* (*pCreator)());
	void RegisterSaveGameFactory(const char* name, ISaveGame* (*pCreator)());

	bool SaveGame(GameFramework* pGameFramework, const char* method, const char* name,
		ESaveGameReason reason, const char* checkPoint);

	enum class LoadResult
	{
		OK = 0, FAILED = 1, FAILED_AND_DESTROYED_STATE = 2
	};

	LoadResult LoadGame(GameFramework* pGameFramework, const char* method,
		const char* path, SGameStartParams& params, bool requireQuickLoad, bool loadingSaveGame);
};
