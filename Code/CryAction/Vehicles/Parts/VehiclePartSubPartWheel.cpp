#include "VehiclePartSubPartWheel.h"

VehiclePartSubPartWheel::VehiclePartSubPartWheel()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3055fe60;
#else
	std::uintptr_t ctor = 0x30548a20;
#endif

	(this->*reinterpret_cast<void(VehiclePartSubPartWheel::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehiclePartSubPartWheel::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehiclePartSubPartWheel::GetId()
{
	return {};
}

void VehiclePartSubPartWheel::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehiclePartSubPartWheel::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehiclePart
////////////////////////////////////////////////////////////////////////////////

void VehiclePartSubPartWheel::PostInit()
{
}

void VehiclePartSubPartWheel::PostSerialize()
{
}

void VehiclePartSubPartWheel::Reset()
{
}

void VehiclePartSubPartWheel::Release()
{
}

void VehiclePartSubPartWheel::GetMemoryStatistics(ICrySizer*)
{
}

IVehiclePart* VehiclePartSubPartWheel::GetParent(bool root)
{
	return {};
}

const string& VehiclePartSubPartWheel::GetName()
{
	static const CryStringT<char> name{};
	return name;
}

IEntity* VehiclePartSubPartWheel::GetEntity()
{
	return {};
}

void VehiclePartSubPartWheel::OnEvent(const SVehiclePartEvent& event)
{
}

bool VehiclePartSubPartWheel::ChangeState(EVehiclePartState state, int flags)
{
	return {};
}

IVehiclePart::EVehiclePartState VehiclePartSubPartWheel::GetState() const
{
	return {};
}

void VehiclePartSubPartWheel::SetMaterial(IMaterial* pMaterial)
{
}

void VehiclePartSubPartWheel::Physicalize()
{
}

const Matrix34& VehiclePartSubPartWheel::GetLocalTM(bool relativeToParentPart)
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartSubPartWheel::GetLocalBaseTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartSubPartWheel::GetLocalInitialTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartSubPartWheel::GetWorldTM()
{
	static const Matrix34 tm{};
	return tm;
}

void VehiclePartSubPartWheel::SetLocalTM(const Matrix34& localTM)
{
}

void VehiclePartSubPartWheel::SetLocalBaseTM(const Matrix34& localTM)
{
}

const AABB& VehiclePartSubPartWheel::GetLocalBounds()
{
	static const AABB bounds{};
	return bounds;
}

void VehiclePartSubPartWheel::OnHit(Vec3 localPos, float radius, float strength)
{
}

void VehiclePartSubPartWheel::RegisterSerializer(IGameObjectExtension* gameObjectExt)
{
}

int VehiclePartSubPartWheel::GetType()
{
	return {};
}

IVehicleWheel* VehiclePartSubPartWheel::GetIWheel()
{
	return {};
}

void VehiclePartSubPartWheel::AddChildPart(IVehiclePart* pPart)
{
}

void VehiclePartSubPartWheel::InvalidateTM(bool invalidate)
{
}

void VehiclePartSubPartWheel::SetMoveable()
{
}

bool VehiclePartSubPartWheel::GetRotationLimits(int axis, float& min, float& max)
{
	return {};
}

const Vec3& VehiclePartSubPartWheel::GetDetachBaseForce()
{
	static const Vec3 force{};
	return force;
}

float VehiclePartSubPartWheel::GetMass()
{
	return {};
}

int VehiclePartSubPartWheel::GetPhysId()
{
	return {};
}

int VehiclePartSubPartWheel::GetSlot()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartSubPartWheel::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "SubPartWheel", VehiclePartSubPartWheel, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30924c84) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a613c) = id;
#endif
}
