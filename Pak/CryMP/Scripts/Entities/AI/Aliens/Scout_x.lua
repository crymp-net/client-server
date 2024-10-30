Script.ReloadScript("Scripts/Entities/AI/Shared/BasicAI.lua");
Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicAlien.lua");

Scout_x =
{
	AnimationGraph = "Scout-new.xml",

	voiceType = "scout",

	foreignCollisionDamageMult = 0.05,
	isAlien = 1,
	--AI related

	AIType = AIOBJECT_2D_FLY,

	PropertiesInstance =
	{
		aibehavior_behaviour = "ScoutMOACIdle",
		bAutoDisable = 1,
	},
	--AI related	over

	Properties =
	{
	  bExactPos = 0,
		attackrange = 100,
		accuracy = 1.0,
		species = 2,
		aicharacter_character = "Scout",
		soclasses_SmartObjectClass = "Scout",
		fileModel = "Objects/Characters/Alien/Scout/Scout_base.cdf",
		autoDestructionProbability = 10,	-- out of 100
		maxDeadShellNum = 3,
		strafingPitch =16.0,
		AnimPack = "Alien",

		Perception =
		{
			--how visible am I
			camoScale = 1,
			--movement related parameters
			--VELmultyplier = (velBase + velScale*CurrentVel^2);
			--current priority gets scaled by VELmultyplier
			velBase = 1,
			velScale = .03,
			--fov/angle related
			FOVPrimary = 220,			-- normal fov
			FOVSecondary = 300,		-- periferial vision fov
			--ranges
			sightrange = 100,
			--how heights of the target affects visibility
			--// compare against viewer height
			-- fNewIncrease *= targetHeight/stanceScale
			stanceScale = 1.8,
			-- Equivalent to camo scale, used with thermal vision.
			heatScale = 1.0,
			-- Flag indicating that the agent has thermal vision.
			bThermalVision = 1,
		},

		Damage =
		{
			health = 1500,

			explosionMult = 0.6,
			bulletMult = 0.1,
			shieldExplosionMult = 0.3,
			shieldBulletMult = 0.01,
			AIMult = 0.1,


			DamageMultipliers =
			{

				SCAR = 0.25,
				SOCOM = 0.25,
				FY71 = 0.25,
				Shotgun = 0.5,
				GaussRifle = 1,
				DSG1 = 1,
				LAW = 2,
				FastLightMOAC = 0,
				FastLightMOAR = 0,
				ScoutMOAC = 0,
				ScoutMOAR = 0,

				SideWinder_AscMod = 10000,
				APCCannon_AscMod = 1000,

				SMG = 1,
				AlienMount = 1,
				LAW = 1,
				Hurricane = 1,
				FragGrenade = 1,
				ShiTen = 1,

			},

		},

		Beam =
		{
			force = 1.5,
			criticalDistance = 17,
			holdPosition = {x=0,y=0,z=-20},
		},
	},

	Vulnerability =
  {
    DamageEffects =
    {
      -- todo: replace placeholder fx with teh real ones
      { health = 0.75, effect="alien_special.scout.WoundedPlasma",     attachment="damage_effect_1" },
      { health = 0.45, effect="alien_special.scout.wounded",           attachment="damage_effect_2" },
      { health = 0.25, effect="alien_special.scout.WoundedPlasmaBody", attachment="damage_effect_3" },
    },
  },

	physicsParams =
	{
		mass = 10000,

		Living =
		{
			gravity = 1,
			mass = 10000,
			air_resistance = 0.0,
		},
	},

	gameParams =
	{
		stance =
		{
			{
				stanceId = STANCE_STAND,
				normalSpeed = 11.5,
				maxSpeed = 23.0,
				heightCollider = 0.0,
				heightPivot = -1.0,
				size = {x=2.75,y=2.75,z=2.0},
				viewOffset = {x=0,y=0,z=0.0},
				name = "fly",
			},
		},

		-- among these paramter, only speedInertia is applied 22/01/2007 Tetsuji

		speedInertia = 10.0,--the more, the faster the speed change: 1 is very slow, 10 is very fast already
		rotationSpeed = 30.0,

		rollAmount = 0.5,
		rollSpeed = 1.0,

		sprintMultiplier = 1.2,--speed is multiplied by this ammount when alien is sprinting
		sprintDuration = 0.4,--how long the sprint is

		rotSpeed_min = 30.0,--rotation speed at min speed
		rotSpeed_max = 30.0,--rotation speed at max speed

		speed_min = 0.0,--used by the above parameters

		forceView = 20.0,--multiply how much the view is taking into account when moving

		--graphics related
		modelOffset = {x=0,y=0,z=0},
		movingBend = 60,--degrees, how much it will be bend when moving at max speed

		--tentacles
		fullAnimationTentacles = "rope1;rope2;rope05;rope6",
		fullAnimationTentaclesBlendMult = 0,--15,
		tentaclesJointLimit = 15.0,
		--tentaclesCollide = 1,
		tentaclesMaterial = "mat_alien_shell",

		--camera shake amount is 1 * cameraShakeMultiplier at 0 distance, and its 0 * cameraShakeMultiplier at cameraShakeRange (in meters)
		cameraShakeRange = 90,
		cameraShakeMultiplier = 0.5,

		groundEffect = "scout_trail",
		groundEffectHeight = 18,
		groundEffectHeightScale = 0.5,
		groundEffectMaxSpeed = 15,
		groundEffectBaseScale = 4,

		trailEffect = "alien_special.scout.refractive_trail",
		trailEffectMinSpeed = 0,
		trailEffectMaxSpeedSize = 5,
		trailEffectMaxSpeedCount = 5,

		turnSound = "sounds/alien:scout:acceleration_body",
		turnSoundBone = "Bip01 Head",
		turnSoundMaxVel = 1.5,

		destruct_charge_sound = "Sounds/alien:scout:self_destruct_charge",
		destruct_explode_sound = "Sounds/alien:scout:self_destruct_explode",
		destruct_gound_sound = "sounds/physics:collisions:scout_ground",
		death_explode_sound = "Sounds/alien:scout:death_explode",

		--grabbing
		maxGrabMass = 100001,
		maxGrabVolume = 20000,
	},

	--melee stuff
	melee =
	{
		melee_animations =
		{
			{
				"melee_attack_stab",
--				"melee_attack_02",
				1,
			},
		},

		damage = 150,
		damageSmall = 45,
		damageOffset = {x=0,y=-8,z=-6};		-- Local offset of the damage box
		damageRadius = 17.0,								-- size of the damage box.
		approachLookat = 0.05,
		alignTime = 0.7,
		damageTime = 1.0,
		meleeDir = {x=0,y=0,z=1};
	},

	grabCarryDropParams =
	{
		-- TODO Feb 9, 2007: <pvl> just to work around some weird code in
		-- Scout_x:GrabObject()
		holdPos = {x=0,y=2,z=-20}, -- default position where the grab is holded.
		-- TODO Feb 28, 2007: <pvl> to keep BaseGrabHandler from grabbing the
		-- object immediately.  This way, it waits for the "ObjectGrabbed"
		-- animevent.
		grabDelay = 0.5,

		--carryAnimGraphInput = "lightObject",

		-- FIXME Jan 30, 2007: <pvl> arbitrary
		followBone = "claw_rope6",

		limbs =
		{
			"frontRightTentacle",
		},

		grabAnim =
		{
			animGraphSignal = "grabLightObject",
			releaseIKTime = 0.5,
		},
		dropAnim =
		{
			animGraphSignal = "dropLightObject",
		}
	},
	carryTrooperParams =
	{
		-- TODO Feb 9, 2007: <pvl> just to work around some weird code in
		-- Scout_x:GrabObject()
		holdPos = {x=0,y=0,z=-20}, -- default position where the grab is holded.

		carryAnimGraphInput = "troopers",

		-- FIXME Jan 30, 2007: <pvl> arbitrary
		followBone = "claw_rope1",

		-- NOTE Jan 30, 2007: <pvl> we're not showing the process of Scout
		-- grabbing Troopers in Crysis so there's no grabAnim for them

		dropAnim =
		{
			forceThrow = 4.65
		}
	},

	carryMultipleTroopersParams =
	-- NOTE Mar 21, 2007: <pvl> for comments, see carryTrooperParams
	{
		-- the actual grab/drop params, one table for each trooper to be carried
		params = {
			{
				holdPos = {x=0,y=0,z=-20}, -- default position where the grab is holded.
				carryAnimGraphInput = "troopers",
				followBone = "claw_rope1",
				--grabOffset = { x=0, y=0, z=-0.5 },
				boneGrabOffset = { x=0.7, y=0, z=0 },
				throwVec = { x=0, y=0, z=0 },
				dropAnim =
				{
					forceThrow = 0,
					throwVecDelta = { x=-3.5, y=8, z=-5 },
				},

			},
			{
				holdPos = {x=0,y=0,z=-20}, -- default position where the grab is holded.
				carryAnimGraphInput = "troopers",
				followBone = "claw_rope6",
				boneGrabOffset = { x=0.7, y=0, z=0 },
				throwVec = { x=0, y=0, z=0 },
				dropAnim =
				{
					forceThrow = 0,
					throwVecDelta = { x=3.5, y=8, z=-5 },
				},

			},
			{
				holdPos = {x=0,y=0,z=-20}, -- default position where the grab is holded.
				carryAnimGraphInput = "troopers",
				followBone = "claw_rope2",
				boneGrabOffset = { x=0.7, y=0, z=0 },
				throwVec = { x=0, y=0, z=0 },
				dropAnim =
				{
					forceThrow = 0,
					throwVecDelta = { x=5, y=12, z=-5 },
				},

		},
			{
				holdPos = {x=0,y=0,z=-20}, -- default position where the grab is holded.
				carryAnimGraphInput = "troopers",
				followBone = "claw_rope5",
				boneGrabOffset = { x=0.7, y=0, z=0 },
				throwVec = { x=0, y=0, z=0 },
				dropAnim =
				{
					forceThrow = 0,
					throwVecDelta = { x=-5, y=12, z=-5 },
				},

			}
		}
	},
	-- NOTE Mar 29, 2007: <pvl> used by AI behaviour code (driven by a flow graph)
	-- to accumulate objects to be grabbed which useful in case there's more than
	-- one of those, obviously ;-).  Only after this table has all the objects
	-- to be grabbed simultaneously, the actual grabbing is invoked (GrabMultiple()).
	objects_to_be_grabbed = {},

	IKLimbs =
	{
		{0,"frontLeftTentacle","rope1 seg01","","claw_rope1",0},
		{0,"middleLeftTentacle","rope2 seg01","","claw_rope2",0},
		{0,"backLeftTentacle","rope3 seg01","","claw_rope3",0},
		{0,"backRightTentacle","rope4 seg01","","claw_rope4",0},
		{0,"middleRightTentacle","rope5 seg01","","claw_rope5",0},
		--{0,"frontRightTentacle","rope6 seg01","rope6 seg07","claw_rope6",0},
		{0,"frontRightTentacle","rope6 seg01","","claw_rope6",0},
	},

	scripted_events =
	{
		event1 =
		{
			event = ALIENEVENT_PLAYANIMATION,
			value = "idle_to_combat",
			value2 = 1.35,
			value3 = 0.5,
			value4 = 3,
			value5 = true,
		},
		event2 =
		{
			event = ALIENEVENT_ISVULNERABLE,
			value = 1,
		},
		event3 =
		{
			event = ALIENEVENT_ISVULNERABLE,
			value = 0,
		},
		event4 =
		{
			event = ALIENEVENT_FIREPRIMARY,
			value = 1,
		},
		event5 =
		{
			event = ALIENEVENT_FIREPRIMARY,
			value = 2,
		},
		event6 =
		{
			event = ALIENEVENT_STOPANIMATION,
			value = 3,
		},

		moac_attack =
		{
			{"event1",0.0},
			{"event2",1.5},

			{"event4",2.0},
			{"event5",6.0},

			{"event3",7.0},
			{"event6",7.0},
		},

		moac_attack_long =
		{
--			{"event1",0.0},
			{"event2",0.1},

			{"event4",0.1},
			{"event5",4.9},

			{"event3",5.0},
			{"event6",5.0},
		},

		shield_up =
		{
			{"event3",0.0},
			{"event5",0.0},
		},
	},

	-- the AI movement ability of the vehicle.
	AIMovementAbility =
	{
		walkSpeed = 11.5,
		runSpeed = 20.0,
		sprintSpeed = 30.0,
		maneuverSpeed = 6.0,
		b3DMove = 1,
		minTurnRadius = 5,	-- meters
		maxTurnRadius = 20,	-- meters
		optimalFlightHeight = 8.0,
		minFlightHeight = 5.0,
		maxFlightHeight = 30.0,
    pathFindPrediction = 1.0,
		-- The alien scout air space is divided into three zones, each the height of the attackZoneHeight.
		attackZoneHeight = 12,	-- meters
		bSpecialPathfinder = true,
		pathLookAhead = 10,
		pathSpeedLookAheadPerSpeed = 1.5,
		cornerSlowDown = 1.0,
		pathRadius = 5,
		passRadius = 3.0,		-- the radius used in path finder.
		pathType = AIPATH_SCOUT,
		resolveStickingInTrace = 0,
		pathRegenIntervalDuringTrace = 5,
	},

	Client = {};
	Server = {};

	warmupAutoDestructionTime = 2,
	autoDestructionTime = 3.5,

	lastSpeed = 0,
	iAutoDestructTimer = 0,

}

-----------------------------------------------------------------------------------------------------
function Scout_x:OnResetCustom()

	self.iAutoDestructTimer = 0;

	-- create attachment points
	self:CreateBoneAttachment(0, "Bip01 Head", "damage_effect_1");
	self:CreateBoneAttachment(0, "Bip01 Head", "damage_effect_2");
	self:CreateBoneAttachment(0, "Bip01 Head", "damage_effect_3");
	self:CreateBoneAttachment(0, "Bip01 Head", "dodge");

	--drop the grabbed entity, if there is one
	self:DropGrab();

	self.foreignCollisionDamageMult = 0.05;
	--
	self.firstCrash = nil;
	self.waterCrash = nil;
	--
	self.objects_to_be_grabbed = {}
	self.isVulnerable = self.Properties.bInvulnerable;


	--System.Log("RESET COSTOME");

end

function Scout_x.Client:OnUpdate(frameTime)

	if (self.reImpulse) then
		self.reImpulse = self.reImpulse - frameTime;
		if (self.reImpulse < 0.0) then
			self:ReApplyImpulse();
			self.reImpulse = nil;
		end
	end

	if (self.actor:GetHealth() <= 0) then
		return true;
	end

	self:UpdateArmor(frameTime);

	local maxhealth = self.actor:GetMaxHealth();
	local currenthealth = self.actor:GetHealth();


	if (self.idleSound) then
	  local speed = self:GetSpeed();
	  if (speed ~= self.lastSpeed) then
	    Sound.SetParameterValue(self.idleSound, "rpm_scale", __min(1.0, speed/self.gameParams.stance[1].maxSpeed));
			if ( maxhealth >0.0 ) then
		    Sound.SetParameterValue(self.idleSound, "damage", __max(0.0, 1.0 - currenthealth/maxhealth));
	   end
	    self.lastSpeed = speed;
	  end
	end
end

function Scout_x:UpdateArmor(frameTime)


end

function Scout_x:OnShoot()

--	self:StopAnimation(0,3);
--	self:StartAnimation(0, "primary_weapon_fire_02", 3, 0.15, 0.75, false,true);

	return 1;
end

function Scout_x:ResetAnimation()
	self:StopAnimation(0,3);
end

function Scout_x:DoPlayerSeen()

	BasicAlien.DoPlayerSeen(self);

	self.actor:QueueAnimationState("EnemySight");
	--self:StopAnimation(0,3);
	--self:StartAnimation(0, "fly_enemy_sighting", 3, 0.15, 1.0, false,true);
end

function Scout_x:DoShootWeapon()
	self:PushEvent("moac_attack");
	if (self.voiceTable) then
	end
end

function Scout_x:DoShootWeaponLong()
	self:PushEvent("moac_attack_long");
	if (self.voiceTable) then
	end
end

function Scout_x:DoInvestigate()

	self.actor:QueueAnimationState("Investigate");
	--self:StopAnimation(0,3);
	--self:StartAnimation(0, "fly_idle_break_01", 3, 0.5, 1.0, false,true);
end

function Scout_x:DoPickupTruck()
	self:StopAnimation(0,3);
	self:StartAnimation(0, "truck_arrive", 3, 0.3, 0.5, false,true);
end

function Scout_x:DoHoldTruck()
	self:StopAnimation(0,3);
	self:StartAnimation(0, "truck_idle", 3, 0.15, 1.0, true,true);
end

function Scout_x:DoDropTruck()
	self:StopAnimation(0,3);
	self:StartAnimation(0, "truck_take_off", 3, 0.15, 1.0, false,true);
end

function Scout_x:Kill(ragdoll, shooterId, weaponId)
	--Log ("Scout_x:Kill() called");

		local health = self.actor:GetHealth();
	if ( health > 0 ) then
		Log("ERROR:SCOUT is Killed though he has a health still.");
	end

	BasicAlien.Kill(self,ragdoll,shooterId,weaponId);

	self:EnableSearchBeam(false);

	-- Make the scout a dead projectile.
	self.foreignCollisionDamageMult = 1;

	local item = self.inventory:GetCurrentItem();
	if (item) then
		item.item:Quiet();
	end

	--kill the actor
--	if (ragdoll) then
--		self.actor:RagDollize();
--	end
--
--	self:SetPhysicParams(PHYSICPARAM_SIMULATION, {max_logged_collisions = 1,} );

	-- Notify AI system about this
--		self:TriggerEvent(AIEVENT_AGENTDIED);
--	Script.SetTimer(3000, Scout_x.OnDeath, self);

	if (not self.lastImpulseDir) then
		self.lastImpulseDir = {x=0,y=0,z=0};
	end


	local vel = g_Vectors.temp_v1;
	local pos = g_Vectors.temp_v2;
	local dir = self.lastImpulseDir;

	CopyVector(pos,self:GetWorldPos());
	CopyVector(vel,self:GetVelocity());

	local lastSpeed = LengthVector(vel);
	if (lastSpeed < 13) then
		lastSpeed = 13;
	end

	local bSpecialDir =false;
	local vSpecialDir = { x=0.0,y=0.0,z=0.0 };
	local pathName = AI.GetNearestPathOfTypeInRange( self.id, self:GetPos(), 10000.0, AIAnchorTable.ALIEN_COMBAT_DEATH_PATH, 0.0, 0 );

	if ( pathName ) then

		CopyVector( vSpecialDir,	AI.GetNearestPointOnPath( self.id, pathName, self:GetPos() ) );
		SubVectors( vSpecialDir, vSpecialDir, self:GetPos() );
		vSpecialDir.z =0;
		local len = LengthVector( vSpecialDir );
		NormalizeVector( vSpecialDir );

		if ( len < 200.0 ) then

			local vPathStart = {};
			local vPathEnd = {};
			local vPathDir = {};
			local vPathUp =  { x=0.0,y=0.0,z=1.0 };
			local vPathWng =  {};

			CopyVector( vPathStart, AI.GetPointOnPathBySegNo( self.id, pathName, 0 ) );
			CopyVector( vPathEnd, AI.GetPointOnPathBySegNo( self.id, pathName, 100 ) );

			SubVectors( vPathDir, vPathStart, vPathEnd );
			vPathDir.z =0.0;
			NormalizeVector( vPathDir );
			crossproduct3d( vPathWng , vPathDir, vPathUp );
			NormalizeVector( vPathWng );

			if ( len > 30.0 ) then
				if ( dotproduct3d( vSpecialDir, vPathWng ) > 0.0 ) then
					FastScaleVector( vPathWng, vPathWng, -1.0 )
				end
			else
				if ( dotproduct3d( vSpecialDir, self:GetDirectionVector(1) ) < 0.0 ) then
					FastScaleVector( vPathWng, vPathWng, -1.0 )
				end
			end

			bSpecialDir =true;
			FastScaleVector( vSpecialDir, vPathWng , 15.0 );
			vSpecialDir.z =10.0
			NormalizeVector( vSpecialDir );

		end

	end


	local shooter = System.GetEntity(shooterId);

	if ( bSpecialDir == true ) then

		CopyVector( dir,  vSpecialDir );

	elseif (shooter) then

		CopyVector(dir, shooter:GetWorldPos());

		local sign1 = 1;
		if (math.random(0,1)==0) then
			sign1 = -1;
		end

		local sign2 = 1;
		if (math.random(0,1)==0) then
			sign2 = -1;
		end

		dir.x = dir.x + sign1 * randomF(7,15);
		dir.y = dir.y + sign2 * randomF(7,15);

		SubVectors(dir,dir,pos);

		NormalizeVector(dir);
		dir.z = randomF(-0.25,0.5);
		NormalizeVector(dir);

		FastSumVectors(dir,dir,vel);
		NormalizeVector(dir);
	else

		FastSumVectors(dir,vel,self.actor:GetHeadDir());
		NormalizeVector(dir);

	end

	if ( bSpecialDir == true ) then

		local stats = self:GetPhysicalStats();
		self.lastImpulseAmt = stats.mass*15.0;
		self.reImpulse = 0.1;

	else

		local stats = self:GetPhysicalStats();
		self.lastImpulseAmt = stats.mass*lastSpeed *0.3;
		self.reImpulse = randomF(1.0,3.0);

	end

	--partId 10 is the head
	self:AddImpulse(10,{x=0,y=0,z=0},self.lastImpulseDir,self.lastImpulseAmt,1);

	--explosion
	Particle.SpawnEffect("alien_special.scout.ScoutCrashA", pos, g_Vectors.v001, 1.5);
	self:PlaySoundEvent(self.gameParams.death_explode_sound, g_Vectors.v000, g_Vectors.v010, 0, SOUND_SEMANTIC_EXPLOSION);
  self:ResetDamageEffects();

	self.InitiateAutoDestruction(self);

	self.actor:SetPhysicalizationProfile("ragdoll");
end

function Scout_x:ScriptEvent(event,value,str)
	if (event == "profileChanged") then
		if (str == "ragdoll") then
			self:SetPhysicParams(PHYSICPARAM_SIMULATION, {max_logged_collisions = 1,} );
		end
	else
		BasicActor.ScriptEvent(self,event,value,str);
	end
end

function Scout_x:GetDamageMultiplier(hit)

	local mult=1;
	local damageT = self.Properties.Damage;

	if(hit.type == "collision") then
		mult = 0.05;
	elseif(hit.weapon and hit.weapon.class and hit.weapon.class ~="") then
		mult = damageT.DamageMultipliers[hit.weapon.class];
		if(not mult) then
			damageT.DamageMultipliers[hit.weapon.class] = 1;
			mult = 1;
		end
	elseif(hit.explosion and hit.impact) then
		-- direct hit with something exploding
		-- check if direct hit
		mult = self.Properties.Damage.DamageMultipliers.LAW;
		local hitpos = hit.pos;
		local abs = math.abs;
		local pos = self:GetPos();
		local stance = self.gameParams.stance[1];
		local size = stance.size;
		if( abs(hitpos.x - pos.x) < size.x and abs(hitpos.y - pos.y) < size.y and abs(hitpos.z  - (pos.z+ stance.heightPivot)) < size.z) then
			mult = mult * 2;
		end
	end

	local shooter = hit.shooter;
	if (not (shooter and shooter.actor and shooter.actor:IsPlayer())) then
		mult = mult * damageT.AIMult;
	end

  return mult;

end

function Scout_x.Client:OnHit(hit, remote)

	if (not BasicAlien.Client.OnHit(self,hit,remote)) then
		return false;
	end

	if ( self.isVulnerable and self.isVulnerable ~=0 ) then
		return false;
	end

	local damageMult = self:GetDamageMultiplier(hit);

	if ( damageMult == 0 ) then
		return false;
	end

	local damage = hit.damage * damageMult;
	local maxhealth = self.actor:GetMaxHealth();

	if ( damage > maxhealth * 0.25 ) then

		local vDir = g_Vectors.temp_v1;
		FastScaleVector( vDir, hit.dir, -1.0 );
		vDir.z = 0.0;
		NormalizeVector( vDir );

		local vFwd = g_Vectors.temp_v2;
		CopyVector( vFwd,self:GetDirectionVector(1) );
		vFwd.z = 0.0;
		NormalizeVector( vFwd );

		local vWng = g_Vectors.temp_v3;
		CopyVector( vWng,self:GetDirectionVector(0) );
		vWng.z = 0.0;
		NormalizeVector( vWng );

		if ( math.abs( dotproduct3d( vFwd, vDir ) ) > math.cos( 3.1416 * 30.0 / 180.0 ) ) then
			self.actor:SetAnimationInput("Signal","largeHit");
		else
			if ( dotproduct3d( vWng, vDir ) > 0 ) then
				self.actor:SetAnimationInput("Signal","HeavyDamageRight");
			else
				self.actor:SetAnimationInput("Signal","HeavyDamageLeft");
			end
		end
	else
		self.actor:SetAnimationInput("Signal","smallHit");
	end

end


function Scout_x.Server:OnHit(hit)--(shooterId, weaponId, matName, damage, hitType, pos, normal, partId, radius)

	if ( self.isVulnerable and self.isVulnerable ~=0 ) then
		local health = self.actor:GetHealth();
		return (health < 1.0);
	end

	local damageMult = self:GetDamageMultiplier(hit);
	if ( damageMult == 0 ) then
		local health = self.actor:GetHealth();
		return (health < 1.0);
	end

	--local damageMult = self:GetDamageMultiplier(hit);
	local damage = hit.damage * damageMult;
	local maxhealth = self.actor:GetMaxHealth();

	local shooter = System.GetEntity(hit.shooterId);
	if(shooter~=nil) then
		g_SignalData.id = hit.shooterId;
		g_SignalData.iValue = 0;
		g_SignalData.fValue = damage;

		if ( damage > maxhealth * 0.3 ) then
			g_SignalData.iValue = 1;
		end

		CopyVector(g_SignalData.point, shooter:GetWorldPos());
		if (self.Properties.species ~= shooter.Properties.species) then
			if(self == g_localActor) then
				-- manage here the player's CLeader since player doesn't have a character/behaviour
				AI.Signal(SIGNALFILTER_LEADER,0,"ORD_ATTACK",self.id,g_SignalData);
			else
				AI.Signal(SIGNALFILTER_SENDER,0,"OnEnemyDamage",self.id,g_SignalData);
			end
		elseif (self.Behaviour.OnFriendlyDamage ~= nil) then
			AI.Signal(SIGNALFILTER_SENDER,0,"OnFriendlyDamage",self.id,g_SignalData);
		else
			AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
		end
	else
		g_SignalData.id = 0;
		g_SignalData.iValue = 0;
		g_SignalData.fValue = damage;
		CopyVector(g_SignalData.point,g_Vectors.v000);
		AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
	end

	local health = self.actor:GetHealth();
	if(shooter~=nil) then
		if (self.Properties.species ~= shooter.Properties.species) then

			health = health - damage;
			if ( health < 1.0 ) then
				health = 0;
			end

			self.actor:SetHealth(health);
			self:HealthChanged();

		end
	end

	return (health < 1.0 );

end


function Scout_x:GrabEntity(entityname)

	if (self.GrabId) then
		return;
	end

	local entity = System.GetEntityByName(entityname);

	if (entity) then

		entity:EnablePhysics(false);
		self:AttachChild(entity.id,0);
		entity:SetLocalPos({x=0,y=-0.8,z=-5.65});
		entity:SetLocalAngles({x=0,y=0,z=0});

		self.GrabId = entity.id;
	else
		self.GrabId = nil;
	end
end

function Scout_x:DropGrab()

	if (not self.GrabId) then
		return;
	end

	local entity = System.GetEntity(self.GrabId);

	if (entity) then

		entity:DetachThis();
		entity:EnablePhysics(true);
		entity:AwakePhysics(1);

		entity:SetWorldPos(SumVectors(self:GetWorldPos(),{x=0,y=-0.8,z=-5.65}));
		entity:SetWorldAngles(self:GetWorldAngles());

	end

	self.GrabId = nil;

end

function Scout_x:InitiateAutoDestruction()
	if ( self.iAutoDestructTimer == 0 ) then
	  self.iAutoDestructTimer = 1;
	  Script.SetTimerForFunction( self.warmupAutoDestructionTime*1000, "Scout_x.WarmupAutoDestruct",self);
	end
end

function Scout_x:WarmupAutoDestruct()

	if ( self:GetSpeed() == nil or self:GetPos() == nil ) then
		self:RemoveActor();
		return;
	end

	if ( self.iAutoDestructTimer == 1 ) then
		local Level = System.GetTerrainElevation( self:GetPos() );
		local WaterLevel = AI.IsPointInWaterRegion( self:GetPos() );
		if ( self:GetPos().z < Level + WaterLevel ) then
			if ( self.waterCrash == nil )	then
				self.waterCrash = 0;
				--Particle.SpawnEffect("alien_special.scout.ScoutCrashB_water", self:GetPos(), g_Vectors.v001,1);
				--self:PlaySoundEvent("Sounds/physics:mat_water:mat_event", g_Vectors.v000,g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_LIVING_ENTITY);
			end
		end

		if ( self.waterCrash ~= nil) then
			self.waterCrash = self.waterCrash + 200;
			if ( self.waterCrash > 3000 ) then
				self:GetBonePos("Bip01 Head", g_Vectors.temp_v1);
				Particle.SpawnEffect("alien_special.scout.ScoutExplosion", g_Vectors.temp_v1, g_Vectors.v001, 1);
				self:PlaySoundEvent(self.gameParams.destruct_charge_sound, g_Vectors.v000, g_Vectors.v010, 0, SOUND_SEMANTIC_EXPLOSION);
				self.iAutoDestructTimer = 2;
				Script.SetTimerForFunction(self.autoDestructionTime*1000,"Scout_x.AutoDestruct",self);
			else
			  Script.SetTimerForFunction( 200, "Scout_x.WarmupAutoDestruct",self);
			end
		elseif ( self:GetSpeed() < 1.0  ) then
			self:GetBonePos("Bip01 Head", g_Vectors.temp_v1);
			Particle.SpawnEffect("alien_special.scout.ScoutExplosion", g_Vectors.temp_v1, g_Vectors.v001, 1);
			self:PlaySoundEvent(self.gameParams.destruct_charge_sound, g_Vectors.v000, g_Vectors.v010, 0, SOUND_SEMANTIC_EXPLOSION);
			self.iAutoDestructTimer = 2;
			Script.SetTimerForFunction(self.autoDestructionTime*1000,"Scout_x.AutoDestruct",self);
		else
		  Script.SetTimerForFunction( 200, "Scout_x.WarmupAutoDestruct",self);
		end
	end
end

function Scout_x:AutoDestruct()

	if ( self:GetSpeed() == nil or self:GetPos() == nil ) then
		self:RemoveActor();
		return;
	end

	if ( self.iAutoDestructTimer == 2 ) then

		local pos = g_Vectors.temp_v1;
		self:GetWorldPos(pos);

		g_gameRules:ClientViewShake(pos,75,2,0.25,0.0075);
		g_gameRules:CreateExplosion(self.id,self.id,200,pos,nil,10);
		Sound.Play(self.gameParams.destruct_explode_sound, pos, 0, SOUND_SEMANTIC_EXPLOSION);

		--self.actor:CreateCodeEvent { event="spawnDebris" }

		self:RemoveActor();
		self.iAutoDestructTimer = 0;

	end

end

function Scout_x:ReApplyImpulse()

	local vdir = g_Vectors.temp_v1;
	CopyVector(vdir,self:GetVelocity());
	NormalizeVector(vdir);

	if (not self.lastImpulseDir) then
		self.lastImpulseDir = {x=0,y=0,z=0};
	end

	if (LengthSqVector(vdir)<0.1) then
		CopyVector(vdir,self.lastImpulseDir);
	end

	self:AddImpulse(10,{x=0,y=0,z=0},self.lastImpulseDir,self.lastImpulseAmt,1);
end

function Scout_x:PushEntities(hit)

	if ( not hit.target_id ) then
		return;
	end

	local e = System.GetEntity(hit.target_id);

	if (e) then

		if (e and e.actor and e.actor:IsPlayer()) then

			local dir = e:GetWorldPos();
			SubVectors(dir,dir,self:GetWorldPos());
			dir.z = dir.z + 10.0;
			NormalizeVector(dir);

			e:AddImpulse(-1, e:GetWorldPos(), dir, self:GetSpeed() * 9.81 , 1);

		end
	end
end

function Scout_x.Client:OnCollision(hit)

  if (hit.target_mass and hit.target_mass > 0 and hit.target_mass < 75) then
    return true; -- don't process collisions with light objects
  end

	if (not self.nextCollCheck) then
		self.nextCollCheck = 0;
	end

	if (self.nextCollCheck > _time) then--or hit.target_id
		return true;
	end

	self.nextCollCheck = _time + 0.3;

	self:PushEntities(hit);

	-- the code below is supposed to use for the dead scout

	if ( not self:IsDead() ) then
		return true;
	end

	local vel = LengthSqVector(hit.velocity);

	if (vel>8*8) then

		local pos = g_Vectors.temp_v1;
		CopyVector(pos,self:GetWorldPos());
		pos.z = pos.z + 0.0;

		local spawned = false;
		if (hit.materialId and hit.materialId >= 0) then
			local effectId = MaterialEffects.GetEffectId("scout_crash", hit.materialId);
		  if (effectId ~= MFX_INVALID_EFFECTID) then
		    spawned = MaterialEffects.ExecuteEffect(effectId, {pos=hit.pos});
		  end
		end
		if (not spawned) then
		  Particle.SpawnEffect("alien_special.scout.ScoutCrashB_snow", hit.pos, g_Vectors.v001,1);
--		  Log("fallback: ScoutCrashB_snow");
		end

		if (self.firstCrash) then
		  -- smaller shake for subsequent hits
		  g_gameRules:ClientViewShake(pos,30,1.5,0.25,0.05);

		  -- MR: remove after review (move to MatFX)
		  self:PlaySoundEvent(self.gameParams.destruct_gound_sound, g_Vectors.v000, g_Vectors.v010, 0, SOUND_SEMANTIC_EXPLOSION);
		end

	  if (not self.firstCrash) then
			Particle.SpawnEffect("alien_special.scout.ScoutCrashA", hit.pos, g_Vectors.v001,1);
			self.firstCrash = 1;
			g_gameRules:ClientViewShake(pos,90,3,0.5,0.05);

			-- MR: remove after review (move to MatFX)
			self:PlaySoundEvent(self.gameParams.death_explode_sound, g_Vectors.v000, g_Vectors.v010, 0, SOUND_SEMANTIC_EXPLOSION);
		end

	end
end

-----------------------------------------------------------------------------------------------------
function Scout_x:MakeParalyzed( dir, strength )
	-- dir - direction from the player to the alien, can be used as push direction.
	-- strength - value based on the distance to the center of the blast (1=at players position, 0=at outer range)

	-- The alien paralyze goal pipe will reset the attention target, and make the alien to wait for a moment.
	self:InsertSubpipe(0,"alien_paralyze");

	-- Push the entity around (this may not make any movememnt, depends on the game code).
	local stats = self:GetPhysicalStats();
	self:AddImpulse(-1, pos, dir, stats.mass*2, 1);
end

function Scout_x.OnDeath( entity )
--	AI.LogEvent(" >>>> Scout_x.OnDeath "..entity:GetName());
--	entity:TriggerEvent(AIEVENT_TARGETDEAD);
--	entity:TriggerEvent(AIEVENT_AGENTDIED);
--	entity:Event_OnDeath();
--	entity:Event_OnDead();
end

function Scout_x:GrabObject(object, query)

	--Log ("Scout grabbing "..object.class.."!");
	if (object.class == "Trooper") then
		self.grabParams = self.carryTrooperParams
	else
		self.grabParams = self.grabCarryDropParams
	end

	local beamProp = self.Properties.Beam;
	local grabParams = self.grabParams;

	grabParams.followSpeed = beamProp.force;
	grabParams.criticalDistance = beamProp.criticalDistance;
	CopyVector(grabParams.holdPos,beamProp.holdPosition);

	return BasicActor.GrabObject(self,object,query);

end

------------------------------------------------------------------------------
function Scout_x:EnableSearchBeam(bEnable)
  self.actor:CreateCodeEvent({event="searchBeam", enable=bEnable});
end

------------------------------------------------------------------------------
function Scout_x:SetSearchBeamDir(worldDir)
  self.actor:SetSearchBeam(worldDir);
end


function Scout_x:GrabMultiple()

	self.isVulnerable = 1;
	self.grabParams = { }
	self.grabParams.params = { }

	local numGrabbed = table.getn (self.objects_to_be_grabbed);

	--System.Log("Scout_x:GrabMultiple() for "..self:GetName().."/"..numGrabbed)

	self.grabParams.params[numGrabbed] = self.carryMultipleTroopersParams.params[numGrabbed]
	self.grabParams.params[numGrabbed].entityId = self.objects_to_be_grabbed[numGrabbed];
	AI.Signal(SIGNALFILTER_SENDER,0,"GO_TO_GRABBED",self.objects_to_be_grabbed[numGrabbed]);

	local grabParams = new(self.grabParams);
	grabParams.event = "grabObject";

	if (self.actor:CreateCodeEvent(grabParams)) then
		return 1;
	end

	return 0;
end

function Scout_x:DropObject(throw,throwVec,throwDelay)

	if ( self.objects_to_be_grabbed ==nil ) then
		return;
	end

	local numGrabbed = table.getn (self.objects_to_be_grabbed)
	--System.Log(" numGrabbed "..numGrabbed );

	if (numGrabbed == 0) then
		-- NOTE Mar 26, 2007: <pvl> a legacy, backward-compatibility
		-- code path - having zero objects in objects_to_be_grabbed
		-- actually means that the scout is carrying a single object.
		-- this logic have a problem tetsuji removed below
		-- BasicActor.DropObject(self,throw,throwVec,throwDelay)
		return;
	end

	self:DropMultiple (throw, throwVec, throwDelay)

end

function Scout_x:DropMultiple(throw, throwVec, throwDelay)


	if ( self.objects_to_be_grabbed ==nil ) then
		return;
	end

	local numGrabbed = table.getn (self.objects_to_be_grabbed);
	if ( numGrabbed < 1 ) then
		return;
	end

	local dropParams = self.carryMultipleTroopersParams
	for i=1, numGrabbed do

		local vVel = g_Vectors.temp_v1;
		self:GetVelocity( vVel );
		if ( LengthVector( vVel ) < 1.0 ) then
			CopyVector( vVel, self:GetDirectionVector(1) );
		else
			NormalizeVector( vVel );
		end

		local vUp = g_Vectors.temp_v2;
		vUp.x =0.0;
		vUp.y =0.0;
		vUp.z =1.0;

		local vWng = g_Vectors.temp_v3;
		crossproduct3d( vWng, vVel, vUp );
		NormalizeVector( vWng );
		crossproduct3d( vVel, vUp, vWng );
		NormalizeVector( vVel );

		FastScaleVector( vWng, vWng, dropParams.params[i].dropAnim.throwVecDelta.x );
		FastScaleVector( vVel, vVel, dropParams.params[i].dropAnim.throwVecDelta.y );
		FastScaleVector( vUp,  vUp,  dropParams.params[i].dropAnim.throwVecDelta.z );

		FastSumVectors( vVel, vVel, vWng );
		FastSumVectors( vVel, vVel, vUp );

		CopyVector( dropParams.params[i].throwVec ,vVel );
		dropParams.params[i].throwDelay = 0
		AI.Signal(SIGNALFILTER_SENDER,0,"OnDropped",self.objects_to_be_grabbed[i]);

	end

	dropParams.event = "dropObject"

	self.actor:CreateCodeEvent(dropParams);
	self.objects_to_be_grabbed = {}
	self.isVulnerable = 0;

end



------------------------------------------------------------------------------
function Scout_x:EnableSearchBeam(bEnable)
  self.actor:CreateCodeEvent({event="searchBeam", enable=bEnable});
end

------------------------------------------------------------------------------
function Scout_x:SetSearchBeamDir(worldDir)
  self.actor:SetSearchBeam(worldDir);
end


function SearchBeam(scoutName, enable)
  local ent = System.GetEntityByName(scoutName);
  ent.EnableSearchBeam(ent, enable);
end

function BeamTest(pickerName,pickedName)
	local picker = System.GetEntityByName(pickerName or "");
	local picked = System.GetEntityByName(pickedName or "");

	if (not picked) then
		picker:DropObject(true);
	else
		picker:GrabObject(picked,nil);
	end
end

--

function ScoutGrab (scoutName, objectName)

	--Log ("ScoutGrab() called");

	local scout = System.GetEntityByName (scoutName);
	local object = System.GetEntityByName (objectName);

	if (scout:GrabObject (object, nil) == 0) then
		--Log ("failed");
	else
		--Log ("succeeded");
	end

end

function ScoutThrow (scoutName)

	--Log ("ScoutThrow() called");

	local scout = System.GetEntityByName (scoutName);
	local throwVec = { x=10, y=0, z=10 };

	scout:DropObject (true, throwVec, 0.0);

end

function Scout_x:OnSave(save)

	save.iAutoDestructTimer = self.iAutoDestructTimer;
	save.lastSpeed = self.lastSpeed;
	save.lastImpulseDir = self.lastImpulseDir;
	save.lastImpulseAmt = self.lastImpulseAmt;

	if ( self.reImpulse ~=nil ) then
		save.reImpulse = self.reImpulse;
	end

	save.objects_to_be_grabbed1 = nil;
	save.objects_to_be_grabbed2 = nil;
	save.objects_to_be_grabbed3 = nil;
	save.objects_to_be_grabbed4 = nil;

	save.isVulnerable = self.isVulnerable;

	local numGrabbed = table.getn (self.objects_to_be_grabbed);

	if ( numGrabbed > 0 ) then
		local e = System.GetEntity( self.objects_to_be_grabbed[1] );
		save.objects_to_be_grabbed1 = e;
	end
	if ( numGrabbed > 1 ) then
		local e = System.GetEntity( self.objects_to_be_grabbed[2] );
		save.objects_to_be_grabbed2 = e;
	end
	if ( numGrabbed > 2 ) then
		local e = System.GetEntity( self.objects_to_be_grabbed[3] );
		save.objects_to_be_grabbed3 = e;
	end
	if ( numGrabbed > 3 ) then
		local e = System.GetEntity( self.objects_to_be_grabbed[4] );
		save.objects_to_be_grabbed4 = e;
	end

end

function Scout_x:OnLoad(save)

	self.iAutoDestructTimer = save.iAutoDestructTimer;
	self.lastSpeed = save.lastSpeed;
	self.lastImpulseDir = save.lastImpulseDir;
	self.lastImpulseAmt = save.lastImpulseAmt;

	if ( save.reImpulse  ~=nil ) then
		self.reImpulse = save.reImpulse;
	end

	self.isVulnerable = save.isVulnerable;

	self.objects_to_be_grabbed ={};
	if ( save.objects_to_be_grabbed1 ~=nil ) then
		self.objects_to_be_grabbed[1] = save.objects_to_be_grabbed1.id;
	end
	if ( save.objects_to_be_grabbed2 ~=nil ) then
		self.objects_to_be_grabbed[2] = save.objects_to_be_grabbed2.id;
	end
	if ( save.objects_to_be_grabbed3 ~=nil ) then
		self.objects_to_be_grabbed[3] = save.objects_to_be_grabbed3.id;
	end
	if ( save.objects_to_be_grabbed4 ~=nil ) then
		self.objects_to_be_grabbed[4] = save.objects_to_be_grabbed4.id;
	end

end
