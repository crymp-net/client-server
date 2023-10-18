#pragma once

class NetworkCVars
{
public:
	int breakageLog = 0;
	float voiceVolume = 0;
	float physPingSmooth = 0;
	float physLagSmooth = 0;
	int physDebug = 0;
	int breakTimeoutFrames = 0;

	NetworkCVars();
	~NetworkCVars();
};
