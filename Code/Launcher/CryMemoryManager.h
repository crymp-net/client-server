#pragma once

struct DLL;
struct IConsole;

namespace CryMemoryManager
{
	struct Statistics {
		bool available = true;
		unsigned long long allocTotalWaste = 0;
		unsigned long long allocTotalAllocations = 0;
		unsigned long long allocActiveAllocations = 0;
		unsigned long long allocActiveMemory = 0;
	};
	void Init(const DLL & CrySystem);
	void RegisterConsoleCommands(IConsole* pConsole);
	void ReleaseConsoleCommands(IConsole* pConsole);
	Statistics GetStatistics();
}
