#pragma once

struct DLL;
struct IConsole;

namespace CryMemoryManager
{
	struct Statistics {
		bool available = true;
		unsigned long long totalWaste = 0;
		unsigned long long totalAllocations = 0;
		unsigned long long activeAllocations = 0;
		unsigned long long activeMemory = 0;
	};
	void Init(const DLL & CrySystem);
	void RegisterConsoleCommands(IConsole* pConsole);
	void ReleaseConsoleCommands(IConsole* pConsole);
	Statistics GetStatistics();
}
