#include "VehicleDamagesTemplateRegistry.h"

void VehicleDamagesTemplateRegistry::InitDefaults()
{
	// Scripts/Entities/Vehicles/DamagesTemplates/DefaultVehicleDamages.xml

	{
		SmartScriptTable group(gEnv->pScriptSystem);
		group->SetValue("name", "VehicleDestroy");
		SmartScriptTable group_damagessubgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
			damagessubgroup->SetValue("delay", 0.0f);
			damagessubgroup->SetValue("randomness", 0.5f);
			SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Destroy");
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Effect");
				SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
				damagebehavior_effect->SetValue("effect", "VehicleDestroyed");
				damagebehavior->SetValue("Effect", damagebehavior_effect);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "SpawnDebris");
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
			group_damagessubgroups->PushBack(damagessubgroup);
		}
		group->SetValue("DamagesSubGroups", group_damagessubgroups);

		m_groups["VehicleDestroy"] = group;
	}
	{
		SmartScriptTable group(gEnv->pScriptSystem);
		group->SetValue("name", "CarDestroy");
		group->SetValue("useTemplate", "VehicleDestroy");
		SmartScriptTable group_damagessubgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
			damagessubgroup->SetValue("delay", 0.2f);
			damagessubgroup->SetValue("randomness", 1.0f);
			SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Explosion");
				SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
				damagebehavior_explosion->SetValue("damage", 350.0f);
				damagebehavior_explosion->SetValue("helper", "vehicleBurnPos");
				damagebehavior_explosion->SetValue("pressure", 3000.0f);
				damagebehavior_explosion->SetValue("radius", 10.0f);
				// Unknown attribute minRadius="5"
				// Unknown attribute physRadius="3"
				// Unknown attribute minPhysRadius="2.2"
				damagebehavior->SetValue("Explosion", damagebehavior_explosion);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
			group_damagessubgroups->PushBack(damagessubgroup);
		}
		{
			SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
			damagessubgroup->SetValue("delay", 3.0f);
			damagessubgroup->SetValue("randomness", 0.25f);
			SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Burn");
				SmartScriptTable damagebehavior_burn(gEnv->pScriptSystem);
				damagebehavior_burn->SetValue("damage", 15.0f);
				damagebehavior_burn->SetValue("helper", "vehicleBurnPos");
				damagebehavior_burn->SetValue("interval", 0.5f);
				damagebehavior_burn->SetValue("radius", 3.0f);
				damagebehavior_burn->SetValue("selfDamage", 15.0f);
				damagebehavior->SetValue("Burn", damagebehavior_burn);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
			group_damagessubgroups->PushBack(damagessubgroup);
		}
		group->SetValue("DamagesSubGroups", group_damagessubgroups);

		m_groups["CarDestroy"] = group;
	}
	{
		SmartScriptTable group(gEnv->pScriptSystem);
		group->SetValue("name", "TankDestroy");
		group->SetValue("useTemplate", "VehicleDestroy");
		SmartScriptTable group_damagessubgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
			damagessubgroup->SetValue("delay", 0.2f);
			damagessubgroup->SetValue("randomness", 1.0f);
			SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Explosion");
				SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
				damagebehavior_explosion->SetValue("damage", 500.0f);
				damagebehavior_explosion->SetValue("helper", "vehicleBurnPos");
				damagebehavior_explosion->SetValue("pressure", 4000.0f);
				damagebehavior_explosion->SetValue("radius", 10.0f);
				// Unknown attribute minRadius="5"
				// Unknown attribute physRadius="3"
				// Unknown attribute minPhysRadius="2.2"
				damagebehavior->SetValue("Explosion", damagebehavior_explosion);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
			group_damagessubgroups->PushBack(damagessubgroup);
		}
		{
			SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
			damagessubgroup->SetValue("delay", 2.0f);
			damagessubgroup->SetValue("randomness", 1.0f);
			SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Burn");
				SmartScriptTable damagebehavior_burn(gEnv->pScriptSystem);
				damagebehavior_burn->SetValue("damage", 22.0f);
				damagebehavior_burn->SetValue("helper", "vehicleBurnPos");
				damagebehavior_burn->SetValue("interval", 0.5f);
				damagebehavior_burn->SetValue("radius", 4.0f);
				damagebehavior_burn->SetValue("selfDamage", 25.0f);
				damagebehavior->SetValue("Burn", damagebehavior_burn);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
			group_damagessubgroups->PushBack(damagessubgroup);
		}
		group->SetValue("DamagesSubGroups", group_damagessubgroups);

		m_groups["TankDestroy"] = group;
	}
	{
		SmartScriptTable group(gEnv->pScriptSystem);
		group->SetValue("name", "BoatDestroy");
		group->SetValue("useTemplate", "VehicleDestroy");
		SmartScriptTable group_damagessubgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
			damagessubgroup->SetValue("delay", 0.2f);
			damagessubgroup->SetValue("randomness", 1.0f);
			SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Explosion");
				SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
				damagebehavior_explosion->SetValue("damage", 300.0f);
				damagebehavior_explosion->SetValue("helper", "vehicleBurnPos");
				damagebehavior_explosion->SetValue("pressure", 2000.0f);
				damagebehavior_explosion->SetValue("radius", 10.0f);
				// Unknown attribute minRadius="5"
				// Unknown attribute physRadius="3"
				// Unknown attribute minPhysRadius="2.2"
				damagebehavior->SetValue("Explosion", damagebehavior_explosion);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Sink");
				damagebehavior->SetValue("damageRatioMin", 1.0f);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
			group_damagessubgroups->PushBack(damagessubgroup);
		}
		{
			SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
			damagessubgroup->SetValue("delay", 1.0f);
			damagessubgroup->SetValue("randomness", 0.0f);
			SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
			{
				SmartScriptTable damagebehavior(gEnv->pScriptSystem);
				damagebehavior->SetValue("class", "Sink");
				damagebehavior->SetValue("damageRatioMin", 1.0f);
				damagessubgroup_damagebehaviors->PushBack(damagebehavior);
			}
			damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
			group_damagessubgroups->PushBack(damagessubgroup);
		}
		group->SetValue("DamagesSubGroups", group_damagessubgroups);

		m_groups["BoatDestroy"] = group;
	}
}
