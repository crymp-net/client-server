#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

struct ISystem;
struct IEntitySystem;

class VehicleSystem : public IVehicleSystem
{
	void* m_reserved[64 - 1] = {};

public:
	explicit VehicleSystem(ISystem* pSystem, IEntitySystem* pEntitySystem);
	virtual ~VehicleSystem();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleSystem
	////////////////////////////////////////////////////////////////////////////////

	void RegisterFactory(const char* name, IVehicleMovement* (*)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleView* (*)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehiclePart* (*)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleDamageBehavior* (*)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleSeatAction* (*)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleAction* (*)(), bool isAI) override;

	bool Init() override;
	void Release() override;
	void Reset() override;

	void RegisterVehicles(IGameFramework* pGameFramework) override;

	IVehicle* CreateVehicle(
		uint16 channelId,
		const char* name,
		const char* vehicleClass,
		const Vec3& pos,
		const Quat& rot,
		const Vec3& scale,
		EntityId id) override;

	IVehicle* GetVehicle(EntityId entityId) override;
	IVehicle* GetVehicleByChannelId(uint16 channelId) override;

	bool IsVehicleClass(const char* name) const override;

	IVehicleMovement* CreateVehicleMovement(const string& name) override;
	IVehicleView* CreateVehicleView(const string& name) override;
	IVehiclePart* CreateVehiclePart(const string& name) override;
	IVehicleDamageBehavior* CreateVehicleDamageBehavior(const string& name) override;
	IVehicleSeatAction* CreateVehicleSeatAction(const string& name) override;
	IVehicleAction* CreateVehicleAction(const string& name) override;

	IVehicleDamagesTemplateRegistry* GetDamagesTemplateRegistry() override;

	bool GetVehicleLightDefaults(const char* type, SmartScriptTable& table) override;

	void GetVehicleImplementations(SVehicleImpls& impls) override;
	bool GetOptionalScript(const char* vehicleName, char* buf, size_t len) override;

	void AddVehicle(EntityId entityId, IVehicle* pProxy) override;
	void RemoveVehicle(EntityId entityId) override;

	TVehicleObjectId AssignVehicleObjectId() override;

	uint32 GetVehicleCount() override;
	IVehicleIteratorPtr CreateVehicleIterator() override;

	IVehicleClient* GetVehicleClient() override;

	void RegisterVehicleClient(IVehicleClient* pVehicleClient) override;

	////////////////////////////////////////////////////////////////////////////////
};
