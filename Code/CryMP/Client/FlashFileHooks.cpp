#include <cstddef>
#include <cstdint>
#include <cstring>

#include "CryCommon/CrySystem/ISystem.h"
#include "Launcher/Launcher.h"
#include "Library/Util.h"
#include "Library/WinAPI.h"

#include "FlashFileHooks.h"

////////////////////////////////////////////////////////////////////////////////

struct FlashRefCount
{
	std::uint32_t refCount = 1;
	void* something1 = nullptr;
	void* something2 = nullptr;

	FlashRefCount()
	{
		void *pCrySystem = gLauncher->GetDLLs().pCrySystem;

#ifdef BUILD_64BIT
		const std::size_t offset = 0x2AE068;
#else
		const std::size_t offset = 0x1E2E68;
#endif

		// additional vtable of some Scaleform reference counter base class
		something1 = WinAPI::RVA(pCrySystem, offset);
	}

	FlashRefCount(const FlashRefCount&) = delete;
	FlashRefCount(FlashRefCount&&) = delete;

	FlashRefCount& operator=(const FlashRefCount&) = delete;
	FlashRefCount& operator=(FlashRefCount&&) = delete;

	virtual ~FlashRefCount() = default;
};

struct FlashFile : FlashRefCount
{
	enum SeekBase
	{
		SeekSet = 0,
		SeekCur = 1,
		SeekEnd = 2,
	};

	virtual const char*  GetPath()                                   = 0;
	virtual bool         IsOpen()                                    = 0;
	virtual bool         Something1()                                { return false; }
	virtual std::int32_t Tell()                                      { return Tell64(); }
	virtual std::int64_t Tell64()                                    = 0;
	virtual std::int32_t GetSize()                                   { return GetSize64(); }
	virtual std::int64_t GetSize64()                                 = 0;
	virtual std::int32_t GetError()                                  = 0;
	virtual std::int32_t Something2()                                { return -1; }
	virtual std::int32_t Read(void* buffer, std::int32_t bufferSize) = 0;
	virtual std::int32_t Skip(std::int32_t size)                     = 0;
	virtual std::int32_t RemainingSize()                             = 0;
	virtual bool         Something3()                                { return true; }
	virtual std::int32_t Seek(std::int32_t offset, SeekBase base)    { return Seek64(offset, base); }
	virtual std::int64_t Seek64(std::int64_t offset, SeekBase base)  = 0;
	virtual bool         Something4()                                { return false; }
	virtual std::int32_t Something5()                                { return -1; }
	virtual bool         Close()                                     = 0;
};

////////////////////////////////////////////////////////////////////////////////

struct HookedFlashFile : public FlashFile
{
	HookedFlashFileDescription hook;
	std::uint64_t pos = 0;

	HookedFlashFile(HookedFlashFileDescription& hook) : hook(hook) {}

	const char*  GetPath()   override { return hook.path.data(); }
	bool         IsOpen()    override { return true; }
	std::int64_t Tell64()    override { return pos; }
	std::int64_t GetSize64() override { return hook.content.size(); }
	std::int32_t GetError()  override { return 0; }

	std::int32_t Read(void* buffer, std::int32_t bufferSize) override
	{
		std::int32_t size = bufferSize;

		if (size < 0 || pos >= hook.content.size())
		{
			size = 0;
		}
		else if ((pos + size) > hook.content.size())
		{
			size = hook.content.size() - pos;
		}

		std::memcpy(buffer, hook.content.data() + pos, size);
		pos += size;

		return size;
	}

	std::int32_t Skip(std::int32_t size) override
	{
		if (size < 0 || pos >= hook.content.size())
		{
			size = 0;
		}
		else if ((pos + size) > hook.content.size())
		{
			size = hook.content.size() - pos;
		}

		pos += size;

		return size;
	}

	std::int32_t RemainingSize() override
	{
		return hook.content.size() - pos;
	}

	std::int64_t Seek64(std::int64_t offset, SeekBase base) override
	{
		if (base == SeekSet)
		{
			pos = 0;
		}
		else if (base == SeekEnd)
		{
			pos = hook.content.size();
		}
		else if (base != SeekCur)
		{
			return -1;
		}

		if (offset < 0 && (-offset) > pos)
		{
			offset = -pos;
		}
		else if ((pos + offset) > hook.content.size())
		{
			offset = hook.content.size() - pos;
		}

		pos += offset;

		return pos;
	}

	bool Close() override { return true; }
};

////////////////////////////////////////////////////////////////////////////////

namespace
{
	FlashFileHooks* g_self;
}

FlashFile* FlashFileHooks::OpenerHook::Open(const char* path)
{
	HookedFlashFileDescription hook;

	if (g_self->Find(path, hook))
	{
		return new HookedFlashFile(hook);
	}
	else
	{
		return (this->*(g_self->m_pOriginalOpenFunc))(path);
	}
}

void FlashFileHooks::HackOpener()
{
	void *pCrySystem = gLauncher->GetDLLs().pCrySystem;

#ifdef BUILD_64BIT
	const std::size_t vtableOffset = 0x279210;
#else
	const std::size_t vtableOffset = 0x1D0354;
#endif

	void **vtable = static_cast<void**>(WinAPI::RVA(pCrySystem, vtableOffset));

	m_pOriginalOpenFunc = reinterpret_cast<TOpenFunc&>(vtable[1]);

	// vtable hook
	TOpenFunc newOpenFunc = &OpenerHook::Open;
	WinAPI::FillMem(&vtable[1], &reinterpret_cast<void*&>(newOpenFunc), sizeof (void*));
}

FlashFileHooks::FlashFileHooks()
{
	if (!g_self)
	{
		HackOpener();
	}

	g_self = this;
}

FlashFileHooks::~FlashFileHooks()
{
}

void FlashFileHooks::Add(const std::string_view& path, const std::string_view& content)
{
	HookedFlashFileDescription hook;
	hook.path = path;
	hook.content = content;

	m_hooks.emplace_back(hook);
}

bool FlashFileHooks::Find(const std::string_view& path, HookedFlashFileDescription& result) const
{
	for (const auto& hook : m_hooks)
	{
		if (Util::EqualNoCase(path, hook.path))
		{
			result = hook;
			return true;
		}
	}

	return false;
}
