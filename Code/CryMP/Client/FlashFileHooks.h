#pragma once

#include <string_view>
#include <vector>

struct FlashFile;

struct HookedFlashFileDescription
{
	std::string_view path;
	std::string_view content;
};

class FlashFileHooks
{
	struct OpenerHook
	{
		FlashFile* Open(const char* path);
	};

	using TOpenFunc = decltype(&OpenerHook::Open);

	TOpenFunc m_pOriginalOpenFunc = nullptr;
	std::vector<HookedFlashFileDescription> m_hooks;

	void HackOpener();

public:
	FlashFileHooks();
	~FlashFileHooks();

	void Add(const std::string_view& path, const std::string_view& content);

	bool Find(const std::string_view& path, HookedFlashFileDescription& result) const;
};
