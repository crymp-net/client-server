#pragma once

class ParticleManager
{


public:

	void OnTick();

	static void CmdListEmitters(IConsoleCmdArgs* pArgs);
	static void CmdListAttachments(IConsoleCmdArgs* pArgs);
	ParticleManager();
	~ParticleManager();


};
