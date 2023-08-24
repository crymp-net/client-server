#include "VehiclePartAnimatedJoint.h"

VehiclePartAnimatedJoint::VehiclePartAnimatedJoint()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3054eaa0;
#else
	std::uintptr_t ctor = 0x3053ac10;
#endif

	(this->*reinterpret_cast<void(VehiclePartAnimatedJoint::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehiclePartAnimatedJoint::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehiclePartAnimatedJoint::GetId()
{
	return {};
}

void VehiclePartAnimatedJoint::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehiclePartAnimatedJoint::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehiclePart
////////////////////////////////////////////////////////////////////////////////

void VehiclePartAnimatedJoint::PostInit()
{
}

void VehiclePartAnimatedJoint::PostSerialize()
{
}

void VehiclePartAnimatedJoint::Reset()
{
}

void VehiclePartAnimatedJoint::Release()
{
}

void VehiclePartAnimatedJoint::GetMemoryStatistics(ICrySizer*)
{
}

IVehiclePart* VehiclePartAnimatedJoint::GetParent(bool root)
{
	return {};
}

const string& VehiclePartAnimatedJoint::GetName()
{
	static const CryStringT<char> name{};
	return name;
}

IEntity* VehiclePartAnimatedJoint::GetEntity()
{
	return {};
}

void VehiclePartAnimatedJoint::OnEvent(const SVehiclePartEvent& event)
{
}

bool VehiclePartAnimatedJoint::ChangeState(EVehiclePartState state, int flags)
{
	return {};
}

IVehiclePart::EVehiclePartState VehiclePartAnimatedJoint::GetState() const
{
	return {};
}

void VehiclePartAnimatedJoint::SetMaterial(IMaterial* pMaterial)
{
}

void VehiclePartAnimatedJoint::Physicalize()
{
}

const Matrix34& VehiclePartAnimatedJoint::GetLocalTM(bool relativeToParentPart)
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartAnimatedJoint::GetLocalBaseTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartAnimatedJoint::GetLocalInitialTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartAnimatedJoint::GetWorldTM()
{
	static const Matrix34 tm{};
	return tm;
}

void VehiclePartAnimatedJoint::SetLocalTM(const Matrix34& localTM)
{
}

void VehiclePartAnimatedJoint::SetLocalBaseTM(const Matrix34& localTM)
{
}

const AABB& VehiclePartAnimatedJoint::GetLocalBounds()
{
	static const AABB bounds{};
	return bounds;
}

void VehiclePartAnimatedJoint::OnHit(Vec3 localPos, float radius, float strength)
{
}

void VehiclePartAnimatedJoint::RegisterSerializer(IGameObjectExtension* gameObjectExt)
{
}

int VehiclePartAnimatedJoint::GetType()
{
	return {};
}

IVehicleWheel* VehiclePartAnimatedJoint::GetIWheel()
{
	return {};
}

void VehiclePartAnimatedJoint::AddChildPart(IVehiclePart* pPart)
{
}

void VehiclePartAnimatedJoint::InvalidateTM(bool invalidate)
{
}

void VehiclePartAnimatedJoint::SetMoveable()
{
}

bool VehiclePartAnimatedJoint::GetRotationLimits(int axis, float& min, float& max)
{
	return {};
}

const Vec3& VehiclePartAnimatedJoint::GetDetachBaseForce()
{
	static const Vec3 force{};
	return force;
}

float VehiclePartAnimatedJoint::GetMass()
{
	return {};
}

int VehiclePartAnimatedJoint::GetPhysId()
{
	return {};
}

int VehiclePartAnimatedJoint::GetSlot()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartAnimatedJoint::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "AnimatedJoint", VehiclePartAnimatedJoint, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x309235f4) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a4b74) = id;
#endif
}
