#pragma once

class GameFrameworkCVars
{
public:
	float playerInteractorRadius = 0;
	int aiLogSignals = 0;
	float aiMaxSignalDuration = 0;

	GameFrameworkCVars();
	~GameFrameworkCVars();
};
