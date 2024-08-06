#pragma once

struct IGameFramework;

class GameSerialize
{
	void* m_reserved[15] = {};

public:
	GameSerialize();

	void RegisterFactories(IGameFramework* pGameFramework);
};
