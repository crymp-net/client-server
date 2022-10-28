#pragma once

class GameWindow
{
	int m_classID = 0;

	static GameWindow s_globalInstance;

public:
	GameWindow();
	~GameWindow();

	// to be removed once we have our own CrySystem
	static GameWindow& GetInstance()
	{
		return s_globalInstance;
	}

	void Init();
	bool OnUpdate();
	void OnQuit();
	void UpdateCursorColor();
};
