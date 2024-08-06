#include "VehiclePartTread.h"

VehiclePartTread::VehiclePartTread()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305630e0;
#else
	std::uintptr_t ctor = 0x3054a890;
#endif

	(this->*reinterpret_cast<void(VehiclePartTread::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehiclePartTread::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehiclePartTread::GetId()
{
	return {};
}

void VehiclePartTread::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehiclePartTread::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehiclePart
////////////////////////////////////////////////////////////////////////////////

void VehiclePartTread::PostInit()
{
}

void VehiclePartTread::PostSerialize()
{
}

void VehiclePartTread::Reset()
{
}

void VehiclePartTread::Release()
{
}

void VehiclePartTread::GetMemoryStatistics(ICrySizer*)
{
}

IVehiclePart* VehiclePartTread::GetParent(bool root)
{
	return {};
}

const string& VehiclePartTread::GetName()
{
	static const CryStringT<char> name{};
	return name;
}

IEntity* VehiclePartTread::GetEntity()
{
	return {};
}

void VehiclePartTread::OnEvent(const SVehiclePartEvent& event)
{
}

bool VehiclePartTread::ChangeState(EVehiclePartState state, int flags)
{
	return {};
}

IVehiclePart::EVehiclePartState VehiclePartTread::GetState() const
{
	return {};
}

void VehiclePartTread::SetMaterial(IMaterial* pMaterial)
{
}

void VehiclePartTread::Physicalize()
{
}

const Matrix34& VehiclePartTread::GetLocalTM(bool relativeToParentPart)
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartTread::GetLocalBaseTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartTread::GetLocalInitialTM()
{
	static const Matrix34 tm{};
	return tm;
}

const Matrix34& VehiclePartTread::GetWorldTM()
{
	static const Matrix34 tm{};
	return tm;
}

void VehiclePartTread::SetLocalTM(const Matrix34& localTM)
{
}

void VehiclePartTread::SetLocalBaseTM(const Matrix34& localTM)
{
}

const AABB& VehiclePartTread::GetLocalBounds()
{
	static const AABB bounds{};
	return bounds;
}

void VehiclePartTread::OnHit(Vec3 localPos, float radius, float strength)
{
}

void VehiclePartTread::RegisterSerializer(IGameObjectExtension* gameObjectExt)
{
}

int VehiclePartTread::GetType()
{
	return {};
}

IVehicleWheel* VehiclePartTread::GetIWheel()
{
	return {};
}

void VehiclePartTread::AddChildPart(IVehiclePart* pPart)
{
}

void VehiclePartTread::InvalidateTM(bool invalidate)
{
}

void VehiclePartTread::SetMoveable()
{
}

bool VehiclePartTread::GetRotationLimits(int axis, float& min, float& max)
{
	return {};
}

const Vec3& VehiclePartTread::GetDetachBaseForce()
{
	static const Vec3 force{};
	return force;
}

float VehiclePartTread::GetMass()
{
	return {};
}

int VehiclePartTread::GetPhysId()
{
	return {};
}

int VehiclePartTread::GetSlot()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartTread::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Tread", VehiclePartTread, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30924c88) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a6140) = id;
#endif
}
