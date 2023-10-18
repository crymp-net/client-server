#include <cstdint>

#include "FlowSystem.h"

FlowSystem::FlowSystem()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30619fd0;
#else
	std::uintptr_t ctor = 0x305bef80;
#endif

	(this->*reinterpret_cast<void(FlowSystem::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IFlowSystem
////////////////////////////////////////////////////////////////////////////////

void FlowSystem::Release()
{
}

void FlowSystem::Update()
{
}

void FlowSystem::Reset()
{
}

IFlowGraphPtr FlowSystem::CreateFlowGraph()
{
	return {};
}

TFlowNodeTypeId FlowSystem::RegisterType(const char* typeName, IFlowNodeFactoryPtr factory)
{
	return {};
}

const char* FlowSystem::GetTypeName(TFlowNodeTypeId typeId)
{
	return {};
}

TFlowNodeTypeId FlowSystem::GetTypeId(const char* typeName)
{
	return {};
}

IFlowNodeTypeIteratorPtr FlowSystem::CreateNodeTypeIterator()
{
	return {};
}

void FlowSystem::RegisterInspector(IFlowGraphInspectorPtr pInspector, IFlowGraphPtr pGraph)
{
}

void FlowSystem::UnregisterInspector(IFlowGraphInspectorPtr pInspector, IFlowGraphPtr pGraph)
{
}

IFlowGraphInspectorPtr FlowSystem::GetDefaultInspector() const
{
	return {};
}

void FlowSystem::EnableInspecting(bool enable)
{
}

bool FlowSystem::IsInspectingEnabled() const
{
	return {};
}

void FlowSystem::GetMemoryStatistics(ICrySizer*)
{
}

////////////////////////////////////////////////////////////////////////////////
