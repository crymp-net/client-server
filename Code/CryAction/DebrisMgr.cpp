#include <cstdint>

#include "DebrisMgr.h"

DebrisMgr::DebrisMgr()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307fad80;
#else
	std::uintptr_t ctor = 0x30707080;
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
