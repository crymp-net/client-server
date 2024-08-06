#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehiclePartMassBox : public IVehiclePart
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x230 - 0x8] = {};
#else
	unsigned char m_data[0x1dc - 0x4] = {};
#endif

public:
	VehiclePartMassBox();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleEventListener
	////////////////////////////////////////////////////////////////////////////////

	void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override;

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleObject
	////////////////////////////////////////////////////////////////////////////////

	TVehicleObjectId GetId() override;

	void Serialize(TSerialize ser, unsigned aspects) override;

	void Update(const float deltaTime) override;

	////////////////////////////////////////////////////////////////////////////////
	// IVehiclePart
	////////////////////////////////////////////////////////////////////////////////

	void PostInit() override;
	void PostSerialize() override;
	void Reset() override;
	void Release() override;
	void GetMemoryStatistics(ICrySizer*) override;

	IVehiclePart* GetParent(bool root) override;
	const string& GetName() override;
	IEntity* GetEntity() override;

	void OnEvent(const SVehiclePartEvent& event) override;

	bool ChangeState(EVehiclePartState state, int flags) override;
	EVehiclePartState GetState() const override;

	void SetMaterial(IMaterial* pMaterial) override;

	void Physicalize() override;

	const Matrix34& GetLocalTM(bool relativeToParentPart) override;
	const Matrix34& GetLocalBaseTM() override;
	const Matrix34& GetLocalInitialTM() override;
	const Matrix34& GetWorldTM() override;

	void SetLocalTM(const Matrix34& localTM) override;
	void SetLocalBaseTM(const Matrix34& localTM) override;

	const AABB& GetLocalBounds() override;

	void OnHit(Vec3 localPos, float radius, float strength) override;

	void RegisterSerializer(IGameObjectExtension* gameObjectExt) override;

	int GetType() override;
	IVehicleWheel* GetIWheel() override;

	void AddChildPart(IVehiclePart* pPart) override;

	void InvalidateTM(bool invalidate) override;

	void SetMoveable() override;

	bool GetRotationLimits(int axis, float& min, float& max) override;

	const Vec3& GetDetachBaseForce() override;
	float GetMass() override;
	int GetPhysId() override;
	int GetSlot() override;

	////////////////////////////////////////////////////////////////////////////////

	static void Register(IVehicleSystem* pVehicleSystem);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehiclePartMassBox) == 0x230);
#else
static_assert(sizeof(VehiclePartMassBox) == 0x1dc);
#endif
