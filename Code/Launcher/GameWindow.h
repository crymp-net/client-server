#pragma once

class GameWindow
{
	int m_classID = 0;

public:
	GameWindow();
	~GameWindow();

	void Init();
	bool OnUpdate();
	void OnQuit();

	static void UpdateCursorColor();
};
