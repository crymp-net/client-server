#pragma once

#include <cstdint>

#include "CryCommon/CryMath/Cry_Math.h"

// interface for the skinnable objects (renderer calls its functions to get teh skinning data)
struct ISkinnable
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	//! Renderer calls this function to allow update the video vertex buffers right before the rendering
	virtual std::uint32_t GetSkeletonPose(
		int nLod,
		const Matrix34& RenderMat34,
		QuatTS*& pBoneQuatsL,
		QuatTS*& pBoneQuatsS,
		QuatTS*& pMBBoneQuatsL,
		QuatTS*& pMBBoneQuatsS,
		Vec4 shapeDeformationData[],
		std::uint32_t& DoWeNeedMorphtargets,
		std::uint8_t*& pRemapTable
	) = 0;
};
