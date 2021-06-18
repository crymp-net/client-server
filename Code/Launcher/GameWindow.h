#pragma once

class GameWindow
{
	int m_classID = 0;

public:
	GameWindow();
	~GameWindow();

	void init();
	bool update();
	void onQuit();

	static void UpdateCursorColor();
};
