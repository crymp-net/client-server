#include "VehicleDamagesTemplateRegistry.h"

VehicleDamagesTemplateRegistry::VehicleDamagesTemplateRegistry()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x30843900;
#else
	std::uintptr_t vtable = 0x30760d94;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;

	// TODO: this is not good, implement this class ASAP
	// probably some C++ allocator stuff
#ifdef BUILD_64BIT
	void* magicValue = reinterpret_cast<void*>(0x10);
#else
	void* magicValue = reinterpret_cast<void*>(0x0d);
#endif

	m_reserved2[5] = &m_reserved2[3];
	m_reserved2[6] = &m_reserved2[3];
	m_reserved2[8] = magicValue;
}

VehicleDamagesTemplateRegistry::~VehicleDamagesTemplateRegistry()
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamagesTemplateRegistry
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamagesTemplateRegistry::Init(const string& defaultDefFilename, const string& damagesTemplatesPath)
{
	return {};
}

void VehicleDamagesTemplateRegistry::Release()
{
}

bool VehicleDamagesTemplateRegistry::RegisterTemplates(const string& filename, const string& defFilename)
{
	return {};
}

bool VehicleDamagesTemplateRegistry::UseTemplate(const string& templateName, IVehicleDamagesGroup* pDamagesGroup)
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
