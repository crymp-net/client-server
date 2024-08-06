#include "VehiclePartMassBox.h"

VehiclePartMassBox::VehiclePartMassBox()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3055cf40;
#else
	std::uintptr_t ctor = 0x305464c0;
#endif

	(this->*reinterpret_cast<void(VehiclePartMassBox::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehiclePartMassBox::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehiclePartMassBox::GetId()
{
	return {};
}

void VehiclePartMassBox::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehiclePartMassBox::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehiclePart
////////////////////////////////////////////////////////////////////////////////

void VehiclePartMassBox::PostInit()
{
}

void VehiclePartMassBox::PostSerialize()
{
}

void VehiclePartMassBox::Reset()
{
}

void VehiclePartMassBox::Release()
{
}

void VehiclePartMassBox::GetMemoryStatistics(ICrySizer*)
{
}

IVehiclePart* VehiclePartMassBox::GetParent(bool root)
{
	return {};
}

const string& VehiclePartMassBox::GetName()
{
	static const CryStringT<char> name{};
	return name;
}

IEntity* VehiclePartMassBox::GetEntity()
{
	return {};
}

void VehiclePartMassBox::OnEvent(const SVehiclePartEvent& event)
{
}

bool VehiclePartMassBox::ChangeState(EVehiclePartState state, int flags)
{
	return {};
}

IVehiclePart::EVehiclePartState VehiclePartMassBox::GetState() const
{
	return {};
}

void VehiclePartMassBox::SetMaterial(IMaterial* pMaterial)
{
}

void VehiclePartMassBox::Physicalize()
{
}

const Matrix34& VehiclePartMassBox::GetLocalTM(bool relativeToParentPart)
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartMassBox::GetLocalBaseTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartMassBox::GetLocalInitialTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartMassBox::GetWorldTM()
{
	static const Matrix34 tm{};
	return tm;
}

void VehiclePartMassBox::SetLocalTM(const Matrix34& localTM)
{
}

void VehiclePartMassBox::SetLocalBaseTM(const Matrix34& localTM)
{
}

const AABB& VehiclePartMassBox::GetLocalBounds()
{
	static const AABB bounds{};
	return bounds;
}

void VehiclePartMassBox::OnHit(Vec3 localPos, float radius, float strength)
{
}

void VehiclePartMassBox::RegisterSerializer(IGameObjectExtension* gameObjectExt)
{
}

int VehiclePartMassBox::GetType()
{
	return {};
}

IVehicleWheel* VehiclePartMassBox::GetIWheel()
{
	return {};
}

void VehiclePartMassBox::AddChildPart(IVehiclePart* pPart)
{
}

void VehiclePartMassBox::InvalidateTM(bool invalidate)
{
}

void VehiclePartMassBox::SetMoveable()
{
}

bool VehiclePartMassBox::GetRotationLimits(int axis, float& min, float& max)
{
	return {};
}

const Vec3& VehiclePartMassBox::GetDetachBaseForce()
{
	static const Vec3 force{};
	return force;
}

float VehiclePartMassBox::GetMass()
{
	return {};
}

int VehiclePartMassBox::GetPhysId()
{
	return {};
}

int VehiclePartMassBox::GetSlot()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartMassBox::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "MassBox", VehiclePartMassBox, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30924c78) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a6130) = id;
#endif
}
