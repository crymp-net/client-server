#include "VehiclePartLight.h"

VehiclePartLight::VehiclePartLight()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3055c940;
#else
	std::uintptr_t ctor = 0x30545f80;
#endif

	(this->*reinterpret_cast<void(VehiclePartLight::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehiclePartLight::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehiclePartLight::GetId()
{
	return {};
}

void VehiclePartLight::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehiclePartLight::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehiclePart
////////////////////////////////////////////////////////////////////////////////

void VehiclePartLight::PostInit()
{
}

void VehiclePartLight::PostSerialize()
{
}

void VehiclePartLight::Reset()
{
}

void VehiclePartLight::Release()
{
}

void VehiclePartLight::GetMemoryStatistics(ICrySizer*)
{
}

IVehiclePart* VehiclePartLight::GetParent(bool root)
{
	return {};
}

const string& VehiclePartLight::GetName()
{
	static const CryStringT<char> name{};
	return name;
}

IEntity* VehiclePartLight::GetEntity()
{
	return {};
}

void VehiclePartLight::OnEvent(const SVehiclePartEvent& event)
{
}

bool VehiclePartLight::ChangeState(EVehiclePartState state, int flags)
{
	return {};
}

IVehiclePart::EVehiclePartState VehiclePartLight::GetState() const
{
	return {};
}

void VehiclePartLight::SetMaterial(IMaterial* pMaterial)
{
}

void VehiclePartLight::Physicalize()
{
}

const Matrix34& VehiclePartLight::GetLocalTM(bool relativeToParentPart)
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartLight::GetLocalBaseTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartLight::GetLocalInitialTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartLight::GetWorldTM()
{
	static const Matrix34 tm{};
	return tm;
}

void VehiclePartLight::SetLocalTM(const Matrix34& localTM)
{
}

void VehiclePartLight::SetLocalBaseTM(const Matrix34& localTM)
{
}

const AABB& VehiclePartLight::GetLocalBounds()
{
	static const AABB bounds{};
	return bounds;
}

void VehiclePartLight::OnHit(Vec3 localPos, float radius, float strength)
{
}

void VehiclePartLight::RegisterSerializer(IGameObjectExtension* gameObjectExt)
{
}

int VehiclePartLight::GetType()
{
	return {};
}

IVehicleWheel* VehiclePartLight::GetIWheel()
{
	return {};
}

void VehiclePartLight::AddChildPart(IVehiclePart* pPart)
{
}

void VehiclePartLight::InvalidateTM(bool invalidate)
{
}

void VehiclePartLight::SetMoveable()
{
}

bool VehiclePartLight::GetRotationLimits(int axis, float& min, float& max)
{
	return {};
}

const Vec3& VehiclePartLight::GetDetachBaseForce()
{
	static const Vec3 force{};
	return force;
}

float VehiclePartLight::GetMass()
{
	return {};
}

int VehiclePartLight::GetPhysId()
{
	return {};
}

int VehiclePartLight::GetSlot()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartLight::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Light", VehiclePartLight, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30924c4c) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a6104) = id;
#endif
}
