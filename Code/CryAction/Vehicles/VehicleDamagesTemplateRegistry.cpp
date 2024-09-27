#include "CrySystem/CryLog.h"

#include "VehicleDamagesTemplateRegistry.h"

VehicleDamagesTemplateRegistry::VehicleDamagesTemplateRegistry()
{
}

VehicleDamagesTemplateRegistry::~VehicleDamagesTemplateRegistry()
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamagesTemplateRegistry
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamagesTemplateRegistry::Init(const string& defaultDefFilename, const string& damagesTemplatesPath)
{
	CryLogErrorAlways("%s: Not implemented!", __FUNCTION__);
	return false;
}

void VehicleDamagesTemplateRegistry::Release()
{
	CryLogErrorAlways("%s: Not implemented!", __FUNCTION__);
}

bool VehicleDamagesTemplateRegistry::RegisterTemplates(const string& filename, const string& defFilename)
{
	CryLogErrorAlways("%s: Not implemented!", __FUNCTION__);
	return false;
}

bool VehicleDamagesTemplateRegistry::UseTemplate(const string& templateName, IVehicleDamagesGroup* pDamagesGroup)
{
	const auto it = m_groups.find(std::string_view(templateName.c_str(), templateName.length()));
	if (it == m_groups.end())
	{
		return false;
	}

	return pDamagesGroup->ParseDamagesGroup(it->second);
}

////////////////////////////////////////////////////////////////////////////////
