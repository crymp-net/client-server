#include "VehiclePartAnimated.h"

VehiclePartAnimated::VehiclePartAnimated()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3054d150;
#else
	std::uintptr_t ctor = 0x305397a0;
#endif

	(this->*reinterpret_cast<void(VehiclePartAnimated::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehiclePartAnimated::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehiclePartAnimated::GetId()
{
	return {};
}

void VehiclePartAnimated::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehiclePartAnimated::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehiclePart
////////////////////////////////////////////////////////////////////////////////

void VehiclePartAnimated::PostInit()
{
}

void VehiclePartAnimated::PostSerialize()
{
}

void VehiclePartAnimated::Reset()
{
}

void VehiclePartAnimated::Release()
{
}

void VehiclePartAnimated::GetMemoryStatistics(ICrySizer*)
{
}

IVehiclePart* VehiclePartAnimated::GetParent(bool root)
{
	return {};
}

const string& VehiclePartAnimated::GetName()
{
	static const CryStringT<char> name{};
	return name;
}

IEntity* VehiclePartAnimated::GetEntity()
{
	return {};
}

void VehiclePartAnimated::OnEvent(const SVehiclePartEvent& event)
{
}

bool VehiclePartAnimated::ChangeState(EVehiclePartState state, int flags)
{
	return {};
}

IVehiclePart::EVehiclePartState VehiclePartAnimated::GetState() const
{
	return {};
}

void VehiclePartAnimated::SetMaterial(IMaterial* pMaterial)
{
}

void VehiclePartAnimated::Physicalize()
{
}

const Matrix34& VehiclePartAnimated::GetLocalTM(bool relativeToParentPart)
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartAnimated::GetLocalBaseTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartAnimated::GetLocalInitialTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartAnimated::GetWorldTM()
{
	static const Matrix34 tm{};
	return tm;
}

void VehiclePartAnimated::SetLocalTM(const Matrix34& localTM)
{
}

void VehiclePartAnimated::SetLocalBaseTM(const Matrix34& localTM)
{
}

const AABB& VehiclePartAnimated::GetLocalBounds()
{
	static const AABB bounds{};
	return bounds;
}

void VehiclePartAnimated::OnHit(Vec3 localPos, float radius, float strength)
{
}

void VehiclePartAnimated::RegisterSerializer(IGameObjectExtension* gameObjectExt)
{
}

int VehiclePartAnimated::GetType()
{
	return {};
}

IVehicleWheel* VehiclePartAnimated::GetIWheel()
{
	return {};
}

void VehiclePartAnimated::AddChildPart(IVehiclePart* pPart)
{
}

void VehiclePartAnimated::InvalidateTM(bool invalidate)
{
}

void VehiclePartAnimated::SetMoveable()
{
}

bool VehiclePartAnimated::GetRotationLimits(int axis, float& min, float& max)
{
	return {};
}

const Vec3& VehiclePartAnimated::GetDetachBaseForce()
{
	static const Vec3 force{};
	return force;
}

float VehiclePartAnimated::GetMass()
{
	return {};
}

int VehiclePartAnimated::GetPhysId()
{
	return {};
}

int VehiclePartAnimated::GetSlot()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartAnimated::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Animated", VehiclePartAnimated, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x309235f0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a4b70) = id;
#endif
}
