#include <cstdint>

#include "DebrisMgr.h"

extern std::uintptr_t CRYACTION_BASE;

DebrisMgr::DebrisMgr()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2fad80;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x207080;
#endif

	(this->*reinterpret_cast<void(DebrisMgr::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IDebrisMgr
////////////////////////////////////////////////////////////////////////////////

void DebrisMgr::AddPiece(IEntity*)
{
}

void DebrisMgr::AddPiece(IEntity*, Time expiration_time)
{
}

void DebrisMgr::RemovePiece(EntityId piece)
{
}

void DebrisMgr::Update()
{
}

void DebrisMgr::SetMinExpirationTime(Time)
{
}

void DebrisMgr::SetMaxExpirationTime(Time)
{
}

void DebrisMgr::SetMaxDebrisListLen(int)
{
}

void DebrisMgr::GetMemoryStatistics(ICrySizer* s)
{
}

////////////////////////////////////////////////////////////////////////////////
