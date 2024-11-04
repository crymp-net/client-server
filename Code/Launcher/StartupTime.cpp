#include <chrono>

#include "StartupTime.h"

std::chrono::time_point<std::chrono::steady_clock> g_start = std::chrono::steady_clock::now();
std::chrono::time_point<std::chrono::steady_clock> g_finish;

void StartupTime::Finish()
{
	g_finish = std::chrono::steady_clock::now();
}

double StartupTime::GetSeconds()
{
	return std::chrono::duration<double>(g_finish - g_start).count();
}
