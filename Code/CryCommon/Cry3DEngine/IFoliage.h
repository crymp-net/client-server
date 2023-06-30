#pragma once

#include "CryCommon/CryRenderer/ISkinnable.h"
#include "CryCommon/CryNetwork/SerializeFwd.h"

struct IRenderNode;

struct IFoliage : ISkinnable
{
	enum EFoliageFlags { FLAG_FROZEN=1 };
	virtual int Serialize(TSerialize ser) = 0;
	virtual void SetFlags(int flags) = 0;
	virtual int GetFlags() = 0;
	virtual IRenderNode* GetIRenderNode() = 0;
};
