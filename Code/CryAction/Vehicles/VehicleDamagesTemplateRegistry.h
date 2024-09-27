#pragma once

#include <map>
#include <string>
#include <string_view>

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleDamagesTemplateRegistry : public IVehicleDamagesTemplateRegistry
{
	std::map<std::string, SmartScriptTable, std::less<void>> m_groups;

public:
	VehicleDamagesTemplateRegistry();
	~VehicleDamagesTemplateRegistry();

	// VehicleDamagesTemplateRegistry_XMLData.cpp
	void InitDefaults();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleDamagesTemplateRegistry
	////////////////////////////////////////////////////////////////////////////////

	bool Init(const string& defaultDefFilename, const string& damagesTemplatesPath) override;
	void Release() override;

	bool RegisterTemplates(const string& filename, const string& defFilename) override;
	bool UseTemplate(const string& templateName, IVehicleDamagesGroup* pDamagesGroup) override;

	////////////////////////////////////////////////////////////////////////////////
};
