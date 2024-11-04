#pragma once

#include "CryCommon/CryAction/IFlowSystem.h"

class FlowSystem : public IFlowSystem
{
#ifdef BUILD_64BIT
	unsigned char m_data[0xa0 - 0x8] = {};
#else
	unsigned char m_data[0x5c - 0x4] = {};
#endif

public:
	FlowSystem();

	////////////////////////////////////////////////////////////////////////////////
	// IFlowSystem
	////////////////////////////////////////////////////////////////////////////////

	void Release() override;
	void Update() override;
	void Reset() override;

	IFlowGraphPtr CreateFlowGraph() override;

	TFlowNodeTypeId RegisterType(const char* typeName, IFlowNodeFactoryPtr factory) override;
	const char* GetTypeName(TFlowNodeTypeId typeId) override;
	TFlowNodeTypeId GetTypeId(const char* typeName) override;
	IFlowNodeTypeIteratorPtr CreateNodeTypeIterator() override;

	void RegisterInspector(IFlowGraphInspectorPtr pInspector, IFlowGraphPtr pGraph) override;
	void UnregisterInspector(IFlowGraphInspectorPtr pInspector, IFlowGraphPtr pGraph) override;
	IFlowGraphInspectorPtr GetDefaultInspector() const override;
	void EnableInspecting(bool enable) override;
	bool IsInspectingEnabled() const override;

	void GetMemoryStatistics(ICrySizer*) override;

	////////////////////////////////////////////////////////////////////////////////
};
