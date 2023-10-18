#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>

#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISystem;
struct IEntitySystem;

struct VehicleCVars;

class VehicleSystem : public IVehicleSystem
{
	std::map<std::string, IGameFramework::IVehicleCreator*, std::less<void>> m_vehicleFactories;

	std::map<std::string, IVehicleMovement* (*)(), std::less<void>> m_movementFactories;
	std::map<std::string, IVehicleView* (*)(), std::less<void>> m_viewFactories;
	std::map<std::string, IVehiclePart* (*)(), std::less<void>> m_partFactories;
	std::map<std::string, IVehicleDamageBehavior* (*)(), std::less<void>> m_damageBehaviorFactories;
	std::map<std::string, IVehicleSeatAction* (*)(), std::less<void>> m_seatActionFactories;
	std::map<std::string, IVehicleAction* (*)(), std::less<void>> m_actionFactories;

	std::map<std::string, SmartScriptTable, std::less<void>> m_lightDefaults;

	std::map<EntityId, IVehicle*> m_vehicles;

	std::unique_ptr<IVehicleDamagesTemplateRegistry> m_pDamagesTemplateRegistry;
	std::unique_ptr<VehicleCVars> m_pCVars;

	IGameFramework* m_pGameFramework = nullptr;
	IVehicleClient* m_pVehicleClient = nullptr;

	TVehicleObjectId m_nextVehicleObjectId = 0;

	// some vehicle seat code stores and reads a pointer from here at offset 0x1f8 (64-bit) and 0xfc (32-bit)
	// looks like some last minute dirty hack
	void* m_reserved[42] = {};

	friend class VehicleIterator;
	friend class ScriptBind_VehicleSystem;

public:
	explicit VehicleSystem(IGameFramework* pGameFramework);
	virtual ~VehicleSystem();

	void RegisterVehicleFactory(const char* name, IGameFramework::IVehicleCreator* pCreator);

	void Reload();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleSystem
	////////////////////////////////////////////////////////////////////////////////

	void RegisterFactory(const char* name, IVehicleMovement* (*pFactory)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleView* (*pFactory)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehiclePart* (*pFactory)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleDamageBehavior* (*pFactory)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleSeatAction* (*pFactory)(), bool isAI) override;
	void RegisterFactory(const char* name, IVehicleAction* (*pFactory)(), bool isAI) override;

	bool Init() override;
	void Release() override;
	void Reset() override;

	void RegisterVehicles(IGameFramework* pGameFramework) override;

	IVehicle* CreateVehicle(
		std::uint16_t channelId,
		const char* name,
		const char* vehicleClass,
		const Vec3& pos,
		const Quat& rot,
		const Vec3& scale,
		EntityId id) override;

	IVehicle* GetVehicle(EntityId entityId) override;
	IVehicle* GetVehicleByChannelId(std::uint16_t channelId) override;

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

	std::uint32_t GetVehicleCount() override;
	IVehicleIteratorPtr CreateVehicleIterator() override;

	IVehicleClient* GetVehicleClient() override;

	void RegisterVehicleClient(IVehicleClient* pVehicleClient) override;

	////////////////////////////////////////////////////////////////////////////////

private:
	void InitDamagesTemplateRegistry();
	void InitLightDefaults();
};
