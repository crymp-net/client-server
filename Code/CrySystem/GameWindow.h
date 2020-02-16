/**
 * @file
 * @brief Game window.
 */

#pragma once

class GameWindow
{
	int m_classID;

public:
	GameWindow();
	~GameWindow();

	bool init();
	bool update();

	void onQuit();
};
