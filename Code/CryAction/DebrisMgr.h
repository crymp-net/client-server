#pragma once

#include "CryCommon/CryAction/IDebrisMgr.h"

class DebrisMgr : public IDebrisMgr
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x50 - 0x8] = {};
#else
	unsigned char m_data[0x30 - 0x4] = {};
#endif

public:
	DebrisMgr();

	////////////////////////////////////////////////////////////////////////////////
	// IDebrisMgr
	////////////////////////////////////////////////////////////////////////////////

	void AddPiece(IEntity*) override;
	void AddPiece(IEntity*, Time expiration_time) override;

	void RemovePiece(EntityId piece) override;

	void Update() override;

	void SetMinExpirationTime(Time) override;
	void SetMaxExpirationTime(Time) override;

	void SetMaxDebrisListLen(int) override;

	void GetMemoryStatistics(ICrySizer* s) override;

	////////////////////////////////////////////////////////////////////////////////
};
