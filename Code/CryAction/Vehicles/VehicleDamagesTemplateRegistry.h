#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleDamagesTemplateRegistry : public IVehicleDamagesTemplateRegistry
{
	CryStringT<char> m_reserved1;
	void* m_reserved2[9] = {};

public:
	VehicleDamagesTemplateRegistry();
	virtual ~VehicleDamagesTemplateRegistry();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleDamagesTemplateRegistry
	////////////////////////////////////////////////////////////////////////////////

	bool Init(const string& defaultDefFilename, const string& damagesTemplatesPath) override;
	void Release() override;

	bool RegisterTemplates(const string& filename, const string& defFilename) override;
	bool UseTemplate(const string& templateName, IVehicleDamagesGroup* pDamagesGroup) override;

	////////////////////////////////////////////////////////////////////////////////
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehicleDamagesTemplateRegistry) == 0x58);
#else
static_assert(sizeof(VehicleDamagesTemplateRegistry) == 0x2c);
#endif
