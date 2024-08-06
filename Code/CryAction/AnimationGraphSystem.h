#pragma once

#include "CryCommon/CryAction/IAnimationGraph.h"
#include "CryCommon/CryAction/IAnimationGraphSystem.h"

struct IGameFramework;

class AnimationGraphSystem : public IAnimationGraphSystem
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x288 - 0x8] = {};
#else
	unsigned char m_data[0x1d0 - 0x4] = {};
#endif

public:
	AnimationGraphSystem();

	void RegisterFactories(IGameFramework* pGameFramework);

	IAnimationGraphPtr LoadGraph(const char* filename, bool reload, bool loadBinary);

	////////////////////////////////////////////////////////////////////////////////
	// IAnimationGraphSystem
	////////////////////////////////////////////////////////////////////////////////

	IAnimationGraphCategoryIteratorPtr CreateCategoryIterator() override;
	IAnimationGraphStateFactoryIteratorPtr CreateStateFactoryIterator() override;
	bool TrialAnimationGraph(const char* name, XmlNodeRef animGraph, bool loadBinary) override;
	void FindDeadInputValues(IAnimationGraphDeadInputReportCallback* pCallback, XmlNodeRef animGraph) override;

	void SetAnimationGraphActivation(EntityId id, bool bActivation) override;

	IAnimationGraphStateIteratorPtr CreateStateIterator(EntityId id) override;
	IAnimationGraphInputsPtr RetrieveInputs(EntityId id) override;
	void ExportXMLToBinary(const char* filename, XmlNodeRef animGraph) override;

	////////////////////////////////////////////////////////////////////////////////
};
