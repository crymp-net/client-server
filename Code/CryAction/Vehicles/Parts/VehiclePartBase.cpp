#include "VehiclePartBase.h"

VehiclePartBase::VehiclePartBase()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30559840;
#else
	std::uintptr_t ctor = 0x30543920;
#endif

	(this->*reinterpret_cast<void(VehiclePartBase::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehiclePartBase::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehiclePartBase::GetId()
{
	return {};
}

void VehiclePartBase::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehiclePartBase::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehiclePart
////////////////////////////////////////////////////////////////////////////////

void VehiclePartBase::PostInit()
{
}

void VehiclePartBase::PostSerialize()
{
}

void VehiclePartBase::Reset()
{
}

void VehiclePartBase::Release()
{
}

void VehiclePartBase::GetMemoryStatistics(ICrySizer*)
{
}

IVehiclePart* VehiclePartBase::GetParent(bool root)
{
	return {};
}

const string& VehiclePartBase::GetName()
{
	static const CryStringT<char> name{};
	return name;
}

IEntity* VehiclePartBase::GetEntity()
{
	return {};
}

void VehiclePartBase::OnEvent(const SVehiclePartEvent& event)
{
}

bool VehiclePartBase::ChangeState(EVehiclePartState state, int flags)
{
	return {};
}

IVehiclePart::EVehiclePartState VehiclePartBase::GetState() const
{
	return {};
}

void VehiclePartBase::SetMaterial(IMaterial* pMaterial)
{
}

void VehiclePartBase::Physicalize()
{
}

const Matrix34& VehiclePartBase::GetLocalTM(bool relativeToParentPart)
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartBase::GetLocalBaseTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartBase::GetLocalInitialTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartBase::GetWorldTM()
{
	static const Matrix34 tm{};
	return tm;
}

void VehiclePartBase::SetLocalTM(const Matrix34& localTM)
{
}

void VehiclePartBase::SetLocalBaseTM(const Matrix34& localTM)
{
}

const AABB& VehiclePartBase::GetLocalBounds()
{
	static const AABB bounds{};
	return bounds;
}

void VehiclePartBase::OnHit(Vec3 localPos, float radius, float strength)
{
}

void VehiclePartBase::RegisterSerializer(IGameObjectExtension* gameObjectExt)
{
}

int VehiclePartBase::GetType()
{
	return {};
}

IVehicleWheel* VehiclePartBase::GetIWheel()
{
	return {};
}

void VehiclePartBase::AddChildPart(IVehiclePart* pPart)
{
}

void VehiclePartBase::InvalidateTM(bool invalidate)
{
}

void VehiclePartBase::SetMoveable()
{
}

bool VehiclePartBase::GetRotationLimits(int axis, float& min, float& max)
{
	return {};
}

const Vec3& VehiclePartBase::GetDetachBaseForce()
{
	static const Vec3 force{};
	return force;
}

float VehiclePartBase::GetMass()
{
	return {};
}

int VehiclePartBase::GetPhysId()
{
	return {};
}

int VehiclePartBase::GetSlot()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartBase::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Base", VehiclePartBase, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x309240e0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a5610) = id;
#endif
}
