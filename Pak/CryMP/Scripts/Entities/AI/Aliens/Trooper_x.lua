Script.ReloadScript("Scripts/Entities/AI/Shared/BasicAI.lua");
Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicAlien.lua");


--if(BulletTypeClass==nil) then
--	BulletTypeClass  = {};
--	BulletTypeClass[0] = "Incendiary";
--	BulletTypeClass[1] = "Shotgun";
--	BulletTypeClass[2] = "Normal";
--	BulletTypeClass[3] = "Sniper";
--	BulletTypeClass[4] = "Gauss";
--end

TrTimerVector1 = {x=0,y=0,z=0};
TrTimerVector2 = {x=0,y=0,z=0};

Trooper_x = {

	AnimationGraph = "Trooper.xml",
	voiceType = "trooper",

	foreignCollisionDamageMult = 0.1,
	selfCollisionDamageMult = 0.01,
	collisionKillVelocity = 20.0,

  isAlien = 1,
	cloakMaterial = "objects/characters/alien/trooper/trooper_transparent",
------------------------------------------------------------------------------------

	PropertiesInstance = {
		aibehavior_behaviour = "TrooperIdle",
--		aibehavior_behaviour = "Job_StandIdle",
		bAutoDisable = 1,
	},

	Properties = {
		AnimPack = "Trooper",
		SoundPack = "Trooper",
	  bExactPos = 0,
		attackrange = 70,
		horizontal_fov = 160,
		accuracy = 1.0,
		bForceOutdoor = 0,
	  bCanSelfDestruct = 0,
		bCorridor = 0,
		bIndoor = 0,
		species = 2,
		aicharacter_character = "Trooper",
		soclasses_SmartObjectClass = "Trooper",
		--formationType = "attack_surround1",
		fileModel = "Objects/characters/alien/trooper/trooper_base.chr",
		objFrozenModel = "objects/characters/alien/trooper/trooper_frozen_shatter.cgf",

		damageScale = 1,

		Perception =
		{
			-- Equivalent to camo scale, used with thermal vision.
			heatScale = 1.0,
			-- Flag indicating that the agent has thermal vision.
			bThermalVision = 1,
			bulletHitRadius = 20,
		},
		Damage =
		{
			health = 700,
			FallPercentage = 0,
			customGrabDistance =2,

			DamageMultipliers =
			{
--				lightbullet = 0.25,
--				bullet = 0.25,
--				shotgunshell = 1,
--				hurricanebullet = 1,
--				gaussbullet = 1,
--				sniperbullet = 1,
				SCAR = 0.25,
				SOCOM = 0.25,
				FY71 = 0.25,
				Shotgun = 0.5,
				GaussRifle = 1,
				DSG1 = 1,
				FastLightMOAC = 0,
				FastLightMOAR = 0,
				ScoutMOAC = 0,
				ScoutMOAR = 0,
				SMG = 1,
				AlienMount = 1,
				LAW = 1,
				Hurricane = 1,
				FragGrenade = 1,
				ShiTen = 1,
				VehicleUSMachinegun = 1,
				MeleeVehicle = 0.35,
				Fists = 1,
			},

--			DamageDistanceMultipliers =
--			{
--				Shotgun = {maxDist = 20, multiplier =1.2},
--			},

			ImpulseMultipliers =
			{
--				lightbullet = 5,
--				bullet = 5,
--				shotgunshell = 1,
--				hurricanebullet = 2,
--				gaussbullet = 1,
--				sniperbullet = 2,
				SCAR = 5,
				SOCOM = 5,
				FY71 = 0.25,
				Shotgun = 1,
				GaussRifle = 1,
				DSG1 = 2,
				FastLightMOAC = 0,
				FastLightMOAR = 0,
				ScoutMOAC = 0,
				ScoutMOAR = 0,
				SMG = 1,
				AlienMount = 1,
				LAW = 1,
				Hurricane = 1,
				FragGrenade = 1,
				ShiTen = 1,
				VehicleUSMachinegun = 1,
				Fists = 1,
			},
		},


		distanceToHideFrom = 3,

		Explosion = {
			Damage = 400,
			Radius = 15,
		},

		Behavior =
		{
			--
			Melee = 20,
			Defend = 20,
			--the rest of 100 is default range attack
			DefendDistance = 10,
		},

	},

	Vulnerability =
  {
    DamageEffects =
    {
      { health = 0.75, effect="alien_special.Trooper.WoundedPlasma",     attachment="damage_effect_1" },
      { health = 0.45, effect="alien_special.Trooper.wounded",           attachment="damage_effect_1" },
      { health = 0.25, effect="alien_special.Trooper.WoundedPlasmaBody", attachment="damage_effect_1" },
      --{ health = 0.0, effect="alien_special.Trooper.WoundedPlasma_death", attachment="damage_effect_4" },
    },
  },

	physicsParams =
	{
		flags = 0,
		mass = 50,

		Living =
		{
			mass = 50,
			k_air_control = 0,-- makes them use physics only when flying
			gravity = 9.8;
			inertia = 1.1,
			inertiaAccel = 2.1,
		},
	},

	gameParams =
	{
		stance =
		{
			{
				stanceId = STANCE_STAND,
				normalSpeed = 4.0,
				maxSpeed = 6.0,


				heightCollider = 1.2,
				size = {x=0.4,y=0.4,z=0.5},
				heightPivot = 0.0,
				modelOffset = {x=0,y=-0.2,z=0},

				viewOffset = {x=0,y=0.0,z=1.3},
				weaponOffset = {x=0,y=0.25,z=1.3},
				name = "vert",
			},
			{
				stanceId = STANCE_PRONE,
				normalSpeed = 15.0,
				maxSpeed = 30.0,

				heightCollider = 1.0,
				heightPivot = 0.0,
				size = {x=0.4,y=0.4,z=0.5},
				modelOffset = {x=0,y=-0.2,z=0.0},
				viewOffset = {x=0,y=0,z=1},
				weaponOffset = {x=0,y=0.25,z=1},
				name = "prone",
			},
			{
				stanceId = STANCE_CROUCH,
				normalSpeed = 3.0,
				maxSpeed = 6.0,

				heightCollider = 1.2,
				size = {x=0.4,y=0.4,z=0.5},
				heightPivot = 0.0,
				modelOffset = {x=0,y=-0.2,z=0},

				viewOffset = {x=0,y=0.0,z=0.3},
				weaponOffset = {x=0,y=0.25,z=0.3},
				name = "vert",
			},
			{
				stanceId = STANCE_RELAXED,
				normalSpeed = 4.0,
				maxSpeed = 6.0,
				heightCollider = 1.2,
				size = {x=0.4,y=0.4,z=0.5},
				heightPivot = 0.0,
				modelOffset = {x=0,y=-0.2,z=0},

				viewOffset = {x=0,y=0.0,z=1.3},
				weaponOffset = {x=0,y=0.25,z=1.3},
				name = "vert",
			},
			{
				stanceId = STANCE_STEALTH,
				normalSpeed = 3.0,
				maxSpeed = 6.0,

				heightCollider = 1.2,
				size = {x=0.4,y=0.4,z=0.5},
				heightPivot = 0.0,
				modelOffset = {x=0,y=-0.2,z=0},

				viewOffset = {x=0,y=0.0,z=1.3},
				weaponOffset = {x=0,y=0.25,z=1.3},
				name = "stealth",
			},
			{
				stanceId = STANCE_SWIM,
				normalSpeed = 3.0,
				maxSpeed = 6.0,

				heightCollider = 1.2,
				size = {x=0.4,y=0.4,z=0.5},
				heightPivot = 0.0,
				modelOffset = {x=0,y=-0.2,z=0},

				viewOffset = {x=0,y=0.0,z=1.3},
				weaponOffset = {x=0,y=0.25,z=1.3},
				name = "vert",
			},
		},

		speedInertia = 3.1,--the more, the faster the speed change: 1 is very slow, 10 is very fast already
		steerInertia = 0.3, -- this inertia applies when there is change of velocity - it displaces temporarily the model,
											-- around the entity position, without changing the latter
		rollAmount = 2.3, --3.0,

		sprintMultiplier = 1.6,--speed is multiplied by this ammount when alien is sprinting
		sprintDuration = 0.5,--how long the sprint is

		rotSpeed_min = 2,--min rotation speed (at max speed)
		rotSpeed_max = 3.0,--max rotation speed (at min speed)

		speed_min = 1.0,--used by the above parameters

		forceView = 2.0,--multiply how much the view is taking into account when moving

		tentaclesMaterial = "mat_alien_shell",
		tentaclesRadius = 0.03,
		tentacleStiffnessDecay = 0.1,
		tentacleDampAnim = 1,

		tentaclesJointLimit = 15.0,

		turnSound = "sounds/alien:trooper:turn",
		turnSoundBone = "pelvis",
		turnSoundMaxVel = 1.5,

		groundEffect = "trooper_trail",
		groundEffectHeight = 1.5,
		groundEffectHeightScale = 1.0,
		groundEffectMaxSpeed = 6,
		groundEffectBaseScale = 1.0,

		trailEffect = "alien_special.Trooper.trail",
		trailEffectMinSpeed = 1,
		trailEffectMaxSpeedSize = 0,
		trailEffectMaxSpeedCount = 6,

		heightVarianceLow = 0,
		heightVarianceHigh =0,
	},

	--melee stuff
	melee =
	{
		melee_animations =
		{
			{
				"trooper_meleeAttack",
				1,
			},
		},

		damage = 80,			-- damage when doing melee from front
		damageSmall = 40,	-- damage when doing melee from back
		damageOffset = {x=0,y=2,z=0};		-- Local offset of the damage box
		damageRadius = 4.5,								-- size of the damage box.
		approachLookat = 1,
		alignTime = 0.5,
		damageTime = 0.7,
		effect = {
		},
	},

	-- the AI movement ability of the vehicle.
	AIMovementAbility =
	{
		usePredictiveFollowing = 1,
		pathLookAhead =1,
		pathRadius = 0.5,
		pathSpeedLookAheadPerSpeed = 0.2,
		walkSpeed =5.0,
		runSpeed = 8.0,
		sprintSpeed = 26.0,
		maneuverSpeed = 1.0,
		b3DMove = 0,
		minTurnRadius = 0.0,		-- meters
		maxTurnRadius = 1.0,		-- meters
		pathType = AIPATH_TROOPER,
		cornerSlowDown = 0.5,
		slopeSlowDown = 0.1,
		passRadius = 0.5,
		maxAccel = 7.0,
		maxDecel = 3.5,
	},

	landPreparationTime = 0.1,-- used by trooper.cpp
	steerInertia = 1,

	WarmupAutoDestructionTime = 2,
	AutoDestructionTime = 4.7,
	autoDestructionProbability = 10,	-- out of 100
	maxDeadShellNum = 2,
	bAutoDestructing = false,

	-- how fast I forget the target (S-O-M speed)
	forgetTimeTarget = 4.0,
	forgetTimeSeek = 8.0,
	forgetTimeMemory = 8.0,

--	voiceTable = {
--		warmupAutoDestructSoundName = "sounds/alien:trooper:self_destruct_charge",
--		AutoDestructSoundName = "sounds/alien:trooper:exploding",
--	},
	Client = {},
	Server = {},

}

if (g_deadTrooperShells == nil) then
	g_deadTrooperShells = new (DeadShellCache, false)

	-- perform constructor-like init
	g_deadTrooperShells.explosionProbability = Trooper_x.autoDestructionProbability / 100
	g_deadTrooperShells.maxNum = Trooper_x.maxDeadShellNum
end

TROOPER_MELEE_DAMAGE = 200;
TROOPER_MELEE_DAMAGE2 = 201;
TROOPER_MELEE_DAMAGE = 200;
TROOPER_MELEE_DAMAGE2 = 201;
TROOPER_ACCELERATION_TIMER = 1000;
TROOPER_JUMP_TIMER = 1001;
TROOPER_END_JUMP_TIMER = 1002;
TROOPER_JUMP_CHASE_TIMER = 1003;
TROOPER_END_JUMP_DODGE_TIMER = 1004;
TROOPER_DEPHYSICALIZE_TIMER = 1005;
TROOPER_MELEE_SPECIAL_TIMER = 1006;
TROOPER_CONVERSATION_REQUEST_TIMER = 1007;
TROOPER_CONVERSATION_ANSWER_TIMER = 1008;
TROOPER_CONVERSATION_CHECK_TIMER = 1009;
TROOPER_PLAYERGRABBED_TIMER = 1010;
TROOPER_CHECK_DEAD_SHELL_TIMER = 1011;
TROOPER_GRABBEDFX_TIMER = 1012;
TROOPER_END_MELEE_TIMER = 1013;
TROOPER_WARMUP_AUTODESTRUCT_TIMER = 1014;
TROOPER_AUTODESTRUCT_TIMER = 1015;
TROOPER_WARNING_TIMER = 1016;

TROOPER_CONV_IDLE = 0;
TROOPER_CONV_REQUESTING = 1;


function Trooper_x:CreateAttachments()
	-- create attachment points
	self:CreateBoneAttachment(0, "weapon_bone", "right_item_attachment");
	self:CreateBoneAttachment(0, "Bip01 Head", "damage_effect_1");
--	self:CreateBoneAttachment(0, "Bip01 Head", "damage_effect_2");
--	self:CreateBoneAttachment(0, "Bip01 Head", "damage_effect_3");
	--self:CreateBoneAttachment(0, "Bip01 Head", "damage_effect_4");
	self:CreateBoneAttachment(0, "pelvis", "trail_attachment");
	self:CreateBoneAttachment(0, "Bip01 Head", "Grapped");

end

function Trooper_x:OnResetCustom()

	-- create Leader here, it must be there since the beginning - no matter if trooper is spawned/disabled
	if(not AI.GetLeader(self.id)) then
		AI.SetLeader(self.id);
	end


	self.bAutoDestructing = false;
	self.exploded = false;
	self.bleeding = 0.0;
	self.nextBleed = 0.0;
	self.explosionPos = nil;

	self:Event_UnCloak();
	if(self.searchSound) then
   	self:StopSound(self.searchSound);
   	self.searchSound = nil;
	end
	self.RegisteredWithExplosion =nil;
	self.AI.ChosenTactic = nil;

	self.lastVelMod = 0;

	if(g_deadTrooperShells) then
		g_deadTrooperShells:Reset();
	end

	AIBlackBoard.trooper_ConversationState = nil;

	self:ResetTimers();
	self:KillTimer(TROOPER_DEPHYSICALIZE_TIMER);
	--local meleeEffect = self.melee.effect;
	self.melee.effect[1] = "player_trooper_melee_hit2";
	self.melee.effect[2] = "player_trooper_melee_hit1";
	self.melee.effect[3] = "player_trooper_melee_hit3";

end

--Server:OnUpdate just for testing, to be removed
--function Trooper_x.Server:OnUpdate(frameTime)
--	BasicAlien.Server.OnUpdate(self,frameTime);
--
--	if (self.nextJump) then
--		self.nextJump = self.nextJump - frameTime;
--
--		if (self.nextJump<0.0) then
--			self.nextJump = 3.0;
--			local targetName = self.targetJumpName;
--			if (not targetName) then
--				targetName = "TagPoint1";
--			end
--			local jumpTable = {jumpTo={}};
--			System.GetEntityByName(targetName):GetWorldPos(jumpTable.jumpTo);
--			jumpTable.jumpTo.z = jumpTable.jumpTo.z + 1;
--			self.actor:SetParams(jumpTable);
--		end
--	end
--end

function Trooper_x.Client:OnUpdate(frameTime)

	if (self.bleeding) then
		self.bleeding = self.bleeding - frameTime;

		--self:Bleed(frameTime);

		if (self.bleeding < 0.0) then
			self.bleeding = nil;
		end
	end

	if (self.idleSound ) then
	  local speed = self:GetSpeed();
  	local bodyDir = self:GetDirectionVector(1);
  	local rpmAccel = 0.4;
  	local rpmDecel = 0.2;

  	if(not self.lastBodyDir) then
  		self.lastBodyDir = {};
  	elseif(speed<3) then
	  	-- add also rotation speed
  		local dot = dotproduct3d(self.lastBodyDir,bodyDir);

  		local speedInc = (3-speed)*(1-dot)/(2*math.min(math.max(frameTime,.05),0.3)) * 10;
  		if(speedInc>0.0) then
  			speedInc = math.sqrt(speedInc);
				speed= speed + speedInc;
				rpmAccel = 0.8;
			end
	  end

 		CopyVector(self.lastBodyDir,bodyDir);

		self.speedSoundScale = self.speedSoundScale or 0;
		self.speedSoundScaleGoal = self.speedSoundScaleGoal or 0;

	  if (speed ~= self.lastSpeed and not self.animationEngineSpeed) then
		  local scale = speed/self.gameParams.stance[1].maxSpeed;
		  if(scale>1) then
		  	scale = 1;
		  end
		  scale = 0.2 + scale*0.5;
			--- DEBUG
			local pos = g_Vectors.temp;
			CopyVector(pos,self:GetWorldPos());
			pos.z = pos.z + 2;
			-------

		  self.lastAccelerationCompTime = (self.lastAccelerationCompTime or 0) + frameTime;
		  if (self.lastAccelerationCompTime > 0.3 ) then
		  	if(not self.actor:IsFlying()) then
					local accel = speed/__max(1,(self.lastAccCompSpeed or 0));
					if(accel > 3) then
						--self:PlayAccelerationSound();
--						System.DrawLabel( pos, 3, tostring(accel), 1, 0, 0, 1);
						scale = 0.7 + (__min(9,accel) - 3)/6 *0.3;
					end
				end
		    self.lastAccCompSpeed = speed; -- used to compute acceleration
		  	self.lastAccelerationCompTime = 0;
			end

			if(scale > self.speedSoundScaleGoal) then
		  	-- give priority to high peaks, they must be reached first before decreasing scale again
				self.speedSoundScaleGoal = scale;
			end
			self.newSpeedSoundScaleGoal = scale;
	  end
		local pos = g_Vectors.temp;
		CopyVector(pos,self:GetWorldPos());
		pos.z = pos.z + 1;

		local diff = 0;
		if(self.speedSoundScaleGoal and self.speedSoundScale) then
			diff = self.speedSoundScaleGoal - self.speedSoundScale;
		end

		if(diff <-0.001 ) then
			self.speedSoundScale = __max(self.speedSoundScaleGoal,self.speedSoundScale - frameTime* rpmDecel); -- deceleration
	    Sound.SetParameterValue(self.idleSound, "rpm_scale",  self.speedSoundScale);
		elseif(diff>0.001) then
			self.speedSoundScale = __min(self.speedSoundScaleGoal,self.speedSoundScale + frameTime* rpmAccel); -- acceleration
	    Sound.SetParameterValue(self.idleSound, "rpm_scale",  self.speedSoundScale);
		else
			self.speedSoundScaleGoal = self.newSpeedSoundScaleGoal;
		end
--		System.DrawLabel( pos, 3, tostring(self.speedSoundScale).." - "..tostring(self.speedSoundScaleGoal), 0, 1, 0, 1);
	end
end

function Trooper_x:PlayAccelerationSound()
	if( not (self.accelerationSound and Sound.IsPlaying(self.accelerationSound)))	then
		self.accelerationSound = self:PlaySoundEvent("sounds/alien:trooper:accelerate", g_Vectors.v001, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_LIVING_ENTITY);
--		if (sound) then
--			Sound.SetParameterValue(sound,"speed",1.0);
--		end
	end
end

function Trooper_x:Bleed(frameTime)

	self.nextBleed = self.nextBleed - frameTime;

	if (self.nextBleed < 0.0) then
		local pos = g_Vectors.temp_v1;
		--CopyVector(pos,self:GetWorldPos());
		CopyVector(pos,self:GetBonePos("root"));

		Particle.SpawnEffect("explosions.AlienBlood.b", pos, g_Vectors.v001, 1.0);

		self.nextBleed = randomF(0.05,0.15);
	end
end

-----------------------------------------------------------------------------------------------------

function Trooper_x:Reset()
	BasicAlien.Reset(self);
	self:ResetTimers();
end


function Trooper_x:Kill(ragdoll, shooterId, weaponId)
	if(ragdoll and 	self.exploded) then
		ragdoll = false;
	end
	BasicAlien.Kill(self,ragdoll,shooterId,weaponId);

	Trooper_Death(self);--,autoDestructing); -- let AI do something with this corpse

	self:ResetTimers();

	self:SetAttachmentEffect(0, "damage_effect_1", "alien_special.Trooper.WoundedPlasma_death", g_Vectors.v000, g_Vectors.v010, 1, 0);
	if(self.Properties.bCanSelfDestruct==0) then
		self:SetTimer(TROOPER_CHECK_DEAD_SHELL_TIMER,9000);
    --self:SetAttachmentEffect(0, "damage_effect_1", "alien_special.Trooper.WoundedPlasma_death", g_Vectors.v000, g_Vectors.v010, 1, 0);
	elseif ( not self.bAutoDestructing) then -- and(not g_deadTrooperShells:AddShell(self))) then
		self:InitiateAutoDestruction();
--	else
--    self:SetAttachmentEffect(0, "damage_effect_1", "alien_special.Trooper.WoundedPlasma_death", g_Vectors.v000, g_Vectors.v010, 1, 0);
	end

	if (ragdoll) then
		self.actor:SetPhysicalizationProfile("ragdoll");
		self:SetTimer(TROOPER_DEPHYSICALIZE_TIMER,2000);
	else
		return;
	end

	local vel = g_Vectors.temp_v1;
	local pos = g_Vectors.temp_v2;

	CopyVector(pos,self:GetWorldPos());

	local shooter = System.GetEntity(shooterId);
	if(shooter) then
		FastDifferenceVectors(vel,self:GetPos(),shooter:GetPos());
		NormalizeVector(vel);
	else
		vel = g_Vectors.v000;
	end

	local stats = self:GetPhysicalStats();
--
	self:AddImpulse(-1,pos,vel,stats.mass*5,1);

	if(self.iSoundTimer) then
		Script.KillTimer(self.iSoundTimer);
		self.iSoundTimer = nil;
	end



end

function Trooper_x:ResetTimers()
	self:KillTimer(TROOPER_PLAYERGRABBED_TIMER);
	self:KillTimer(TROOPER_AUTODESTRUCT_TIMER);
	self:KillTimer(TROOPER_WARMUP_AUTODESTRUCT_TIMER);
	self:KillTimer(TROOPER_CONVERSATION_CHECK_TIMER);
	self:KillTimer(TROOPER_CONVERSATION_ANSWER_TIMER);
	self:KillTimer(TROOPER_JUMP_TIMER);
	self:KillTimer(TROOPER_END_JUMP_TIMER);
	self:KillTimer(TROOPER_JUMP_CHASE_TIMER);
	self:KillTimer(TROOPER_END_JUMP_DODGE_TIMER);
	self:KillTimer(TROOPER_MELEE_SPECIAL_TIMER);
	self:KillTimer(TROOPER_CONVERSATION_REQUEST_TIMER);
	self:KillTimer(TROOPER_CHECK_DEAD_SHELL_TIMER);
--	if(self.iAutoDestructTimer) then
--		Script.KillTimer(self.iAutoDestructTimer);
--	end
--	self.iAutoDestructTimer = nil;
	self.bAutoDestructing = false;

	if(self.iSoundTimer) then
		Script.KillTimer(self.iSoundTimer);
	end
	self.iSoundTimer = nil;

end

function Trooper_x.Client:OnHit(hit, remote)

	if(self:GetDamageMultiplier(hit)==0) then
		return
	end

	if (not self.bleeding) then
		self.bleeding = 1.0;
		self.nextBleed = 0.0;
	end

	local shooter = hit.shooter;

--	local distMult = self:GetDamageDistanceMultiplier(hit);
--	hit.damage = hit.damage * distMult;

	if (not BasicAlien.Client.OnHit(self,hit,remote)) then
		return false;
	end

	local damage = hit.damage;


	if(hit.shooter~=nil) then
		g_SignalData.id = hit.shooter.id;
		if(hit.type) then
			g_SignalData.ObjectName = hit.type;
		else
			g_SignalData.ObjectName = "";
		end
		g_SignalData.fValue = damage;
		CopyVector(g_SignalData.point, hit.shooter:GetWorldPos());
		CopyVector(g_SignalData.point2, hit.dir);
		if (self.Properties.species ~= hit.shooter.Properties.species) then
			self.AI.lastEnemyDamageTime = _time;
			AI.Signal(SIGNALFILTER_SENDER,0,"OnEnemyDamage",self.id,g_SignalData);
		elseif (self.Behaviour.OnFriendlyDamage ~= nil) then
			AI.Signal(SIGNALFILTER_SENDER,0,"OnFriendlyDamage",self.id,g_SignalData);
		else
			AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
		end
	else
		g_SignalData.id = 0;
		g_SignalData.fValue = damage;
		CopyVector(g_SignalData.point, g_Vectors.v000);
		AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
	end
end

------------------------------------------------------------------------------
function Trooper_x:GetDamageMultiplier(hit)

	local mult;
	if(hit.weapon and hit.weapon.class and hit.weapon.class ~="") then
		mult = self.Properties.Damage.DamageMultipliers[hit.weapon.class];
		if(mult) then
			return mult;
		else
			self.Properties.Damage.DamageMultipliers[hit.weapon.class] = 1;
		end
	elseif(hit.explosion) then
		if(hit.type == "frag") then
			-- grenade
			return self.Properties.Damage.DamageMultipliers.FragGrenade;
		elseif( hit.impact) then
			-- law / explosive projectile
			-- direct hit with something exploding
			-- check if direct hit
			mult = self.Properties.Damage.DamageMultipliers.LAW;
			local hitpos = hit.pos;
			local abs = math.abs;
			local pos = self:GetPos();
			if( abs(hitpos.x - pos.x) < 0.5 and abs(hitpos.y - pos.y) < 0.5 and abs(hitpos.z  - (pos.z+ 1.2)) < 0.6) then
				mult = mult * 2;
			end
			return mult;
		end
	end
  return 1;
end

------------------------------------------------------------------------------
--function Trooper_x:GetDamageDistanceMultiplier(hit)
--
--	if(hit.weapon and hit.weapon.class and hit.weapon.class ~="") then
--		local multTable = self.Properties.Damage.DamageDistanceMultipliers[hit.weapon.class];
--		if(multTable) then
--			local shooter = hit.shooter;
--			if(shooter) then
--				local dist = self:GetDistance(shooter.id);
--				local maxDist = multTable.maxDist;
--				if(maxDist>0) then
--					if(dist > maxDist) then
--						dist = maxDist;
--					end
--					local mult = multTable.multiplier*(dist/maxDist) + (maxDist - dist)/maxDist;-- *1
--					return mult;
--				end
--			end
--		end
--	end
--  return 1;
--end


function Trooper_x:GetDamage()
	-- used for melee attack
	return random(40,60);
end


function Trooper_x.OnDeathCustom(self)
--	self:RemoveActor();
end


-----------------------------------------------------------------------------------------------------
function Trooper_x:MakeParalyzed( dir, strength )
	-- dir - direction from the player to the alien, can be used as push direction.
	-- strength - value based on the distance to the center of the blast (1=at players position, 0=at outer range)

	-- The alien paralyze goal pipe will reset the attention target, and make the alien to wait for a moment.

	-- Push the entity around (this may not make any movememnt, depends on the game code).
	local stats = self:GetPhysicalStats();
	self:AddImpulse(-1, pos, dir, stats.mass*2, 1);
	AI.Signal(SIGNALFILTER_SENDER,0,"GO_TO_DUMB",self.id);
end


--function Trooper_x:Event_StartAmbush(sender)
--	if(sender) then
--		if(sender.Who) then
--			local target = sender.Who;
--			if(target.id) then
--				g_SignalData.id = target.id;
--			else
--				g_SignalData.id = sender.id;
--			end
--		else
--			g_SignalData.id = sender.id;
--		end
--	else
--		g_SignalData.id = NULL_ENTITY;
--	end
--	AI.Signal(SIGNALFILTER_SUPERGROUP,0,"START_AMBUSH",self.id,g_SignalData);
--end




function Trooper_x.WarmupAutoDestruct(entity)--,timerid)
		local params =
		{
			SpawnPeriod			= 0,
			Scale						= 1,
			CountScale			= 1,
			bCountPerUnit		= 0,
			bSizePerUnit		= 0,
			AttachType			= "none",
			AttachForm			= "none",
			bPrime					= 0,
		}

	local pos = entity:GetPos();
	if(pos) then
		entity.explosionPos = {};
		CopyVector(entity.explosionPos,pos);
	end

	entity:LoadParticleEffect( -1, "alien_special.Trooper.death_chargeup", params );
--	local pos = g_Vectors.temp;
--	CopyVector(pos,entity:GetPos());
	--entity.iAutoDestructTimer = Script.SetTimer(entity.AutoDestructionTime*1000,Trooper_x.AutoDestruct,entity);
	--entity.iWarningTimer = Script.SetTimer(500,Trooper_x.SendWarning,entity);
	entity:SetTimer(TROOPER_AUTODESTRUCT_TIMER,entity.AutoDestructionTime*1000);
	entity:SetTimer(TROOPER_WARNING_TIMER,500);
end

function Trooper_x.SendWarning(entity,timerid)
	local count = AI.GetGroupCount(entity.id);
	if(count>0) then
		local sender = AI.GetGroupMember(entity.id,1);
		if(sender == entity) then
			if(count > 1) then
				sender = AI.GetGroupMember(2);
			else
				entity.iWarningTimer = nil;
				return
			end
		end
		CopyVector(g_SignalData.point,entity:GetWorldPos());
		g_SignalData.iValue = entity.AutoDestructionTime-0.5;
		g_SignalData.fValue = entity.Properties.Explosion.Radius+4;
		AI.Signal(SIGNALFILTER_LEADER,10,"AddDangerPoint",entity.id,g_SignalData);
--		AI.Signal(SIGNALFILTER_SUPERGROUP,1,"OnAvoidDanger",sender.id,g_SignalData);
	end
	entity.iWarningTimer = nil;
end

function Trooper_x.PlayFightSound(entity,timerid)

		entity.warmupAutoDestructSound = entity:PlaySoundEvent("sounds/alien:trooper:threaten", g_Vectors.v000, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_AI_READABILITY);
	entity.iSoundTimer = nil;

end

function Trooper_x:InitiateAutoDestruction()
	self:SetTimer(TROOPER_WARMUP_AUTODESTRUCT_TIMER,self.WarmupAutoDestructionTime*1000);
	--self.iAutoDestructTimer = Script.SetTimer(self.WarmupAutoDestructionTime*1000,Trooper_x.WarmupAutoDestruct,self);
	self.bAutoDestructing = true;
end

function Trooper_x.AutoDestruct(entity)--,timerid)
	entity.iAutoDestructTimer = nil;

	entity.exploded = true;


	local pos = entity:GetWorldPos();
	if(not pos) then
		if(not entity.explosionPos) then
			return;
		end
		pos = g_Vectors.temp_v1;
		CopyVector(pos,entity.explosionPos);
	end
	pos.z = pos.z + 1.0;

	local damage = 	entity.Properties.Explosion.Damage;
	local radius = 	entity.Properties.Explosion.Radius;
	local navtype = AI.GetNavigationType(entity.id);
	if(navtype == NAV_WAYPOINT_HUMAN) then
		local roomSize = AI.GetEnclosingSpace(entity.id,g_Vectors.temp,20,CHECKTYPE_MIN_ROOMSIZE);
		if(roomSize<radius) then
			if(roomSize<3) then
				roomSize = 3;
			end
			damage = damage * roomSize/radius;
			radius = roomSize;
		end
	end
	g_gameRules:CreateExplosion(entity.id,entity.id,damage,pos,nil,radius);
	g_gameRules:ClientViewShake(pos,10,2,0.25,0.0075);
	CopyVector(g_SignalData.point,entity:GetWorldPos());
	entity:NotifyExplosion();
	AI.SetSmartObjectState(entity.id,"Idle");

--	local pos = g_Vectors.temp;
--	CopyVector(pos,entity:GetPos());

	--pos.z = pos.z + 2;

	Particle.SpawnEffect("alien_special.Trooper.death_explosion",pos,g_Vectors.v000,entity.id);
	--entity.warmupAutoDestructSound = entity:PlaySoundEvent("sounds/alien:trooper:self_destruct_charge", g_Vectors.v000, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_LIVING_ENTITY);

	entity:RemoveActor();
end

--function Trooper_x:Cloak(cloak)
--	BasicAlien.Cloak(self,cloak);
--end

function Trooper_x:SetGroupFireModes()
	local groupCount = AI.GetGroupCount(self.id);
	if (groupCount > 1) then
		local i = random(0,1);
		for k=1,groupCount do
			local member = AI.GetGroupMember(self.id,k,GROUP_ENABLED,AIOBJECT_PUPPET);
			if(member) then
				local item = member.inventory:GetCurrentItem();
				if (item.weapon) then
					entity.AI.FireMode = i;
					if(i==1) then
						item.weapon:SetCurrentFireMode("Automatic");
					else
						item.weapon:SetCurrentFireMode("Rapid");
					end
					i = 1-i;
				end
			end
		end
	end
end


function Trooper_x:MeleeAttack(entity)
	if(not self.AI.meleeImpulse) then
		self.AI.meleeImpulse = {x=0,y=0,z=0};
		CopyVector(self.AI.meleeImpulse,self:GetDirectionVector(1));
	end
	self.AI.meleeTarget = entity;
	self:SetGrabbable(0);
	self:SetTimer(TROOPER_END_MELEE_TIMER,1500);
end

function Trooper_x:GetMeleeDamageImpulse(impulse,dirtype)
	if(dirtype==1) then
		CopyVector(impulse, self:GetDirectionVector(0));
		NegVector(impulse);
		FastSumVectors(impulse, impulse,self:GetDirectionVector(1));
	elseif(dirtype==2) then
		CopyVector(impulse, self:GetDirectionVector(0));
		FastSumVectors(impulse, impulse,self:GetDirectionVector(1));
	else
		CopyVector(impulse, self:GetDirectionVector(1));
	end
	impulse.z = impulse.z+0.7;
	NormalizeVector(impulse);
	if(dirtype==4) then
		-- jump melee
		ScaleVectorInPlace(impulse,2);
		self.melee.damageMultiplier = 1.2;
	else
		self.melee.damageMultiplier = 1;
	end
end


function Trooper_x:MeleeDamage(impulse,meleeType)
	local entity = self.AI.meleeTarget;

	local radius;

	if (entity) then
		if(entity.vehicle) then
			if(entity.AI.vehicleRadius) then
				radius = entity.AI.vehicleRadius + 2;
			else
				local bbmin,bbmax = entity:GetLocalBBox(Trooper_bbmin_cache,Trooper_bbmax_cache);
				FastDifferenceVectors(bbmax,bbmax,bbmin);
				local size = bbmax.x;
				local size1 = bbmax.y;
				if(size1 > size) then
					size = size1;
				end
				size = size/2;
				entity.AI.vehicleRadius = size;
				radius = size + 2;
			end
		else
			radius = 2.5;
		end

		local pos = self:GetWorldPos();
		local distance, angle = AI.CheckMeleeDamage(self.id,entity.id,radius,-1.3,1.3,150);

		if(distance) then

			--AI.LogEvent( "Trooper_x:MeleeDamage()" );
			local headDir = self.actor:GetHeadDir();
			local dirX = self:GetDirectionVector(0);
			local dirY = self:GetDirectionVector(1);
			local dirZ = self:GetDirectionVector(2);

			-- if the alien has local meleeDir, blend the head direction and the melee dir.
			local hitDir = g_Vectors.temp_v1;
			if( self.melee.meleeDir ) then
				FastSumVectors( hitDir, headDir, self.melee.meleeDir );
				NormalizeVector( hitDir );
			else
				CopyVector( hitDir, headDir );
			end

--			if(not entity.vehicle) then
				local	offset = self.melee.damageOffset;

				pos.x = pos.x + dirX.x * offset.x + dirY.x * offset.y + dirZ.x * offset.z;
				pos.y = pos.y + dirX.y * offset.x + dirY.y * offset.y + dirZ.y * offset.z;
				pos.z = pos.z + dirX.z * offset.x + dirY.z * offset.y + dirZ.z * offset.z;
--			end
			-- Debug
			--AI.SetRefPointPosition(self.id,pos);
			-- Hit!
			if (g_gameRules and g_gameRules.Client) then -- and g_gameRules.server.RequestMeleeHit) then

				local hit = self.temp_hit;
				--AI.LogComment( "Trooper_x:MeleeDamage: Hit "..self:GetName().."." );
--				if(entity.vehicle) then
--					-- vehicle needs the hit position to actually touch it
--					local dir = g_Vectors.temp;
--					FastDifferenceVectors(dir,entity:GetWorldPos(),pos);
--					local	hits = Physics.RayWorldIntersection(pos,dir,1,ent_static+ent_rigid+ent_sleeping_rigid+ent_living,self.id,nil,g_HitTable);
--					if( hits >0 ) then
--						local firstHit = g_HitTable[1];
--						CopyVector(pos,firstHit.pos);
--					end
--				end

				hit.pos = pos;
				hit.partId = -1;
				hit.dir = hitDir;
				hit.shooter = self;
				hit.shooterId = self.id;
				hit.weapon = self;
				hit.radius = 0;
				hit.weaponId = self.id;
				hit.type 		= "melee";

				local melee = self.melee;
				if(entity.vehicle) then
					hit.damage	= melee.damage * melee.damageMultiplier ;
					hit.damage = hit.damage * self.Properties.Damage.DamageMultipliers.MeleeVehicle;
				elseif( entity.Properties and entity.Properties.bNanoSuit==0) then
					hit.damage	= melee.damage * 4 ;--make melee deadly when there's no nanosuit
				elseif(entity == g_localActor) then
					-- normal melee against player
					local dir = g_Vectors.temp;
					FastDifferenceVectors(dir,self:GetWorldPos(),entity:GetWorldPos());
					local playerDir = g_Vectors.temp_v1;
					g_localActor.actor:GetHeadDir(playerDir);
					local dot = dotproduct2d(dir,playerDir);
					-- check where's the trooper in player fov, more on the back = less damage
					if(dot<0) then
						hit.damage = melee.damageSmall;
					elseif(dot>0.7) then
						hit.damage = melee.damage * melee.damageMultiplier ;
						-- frontal hit, display scratches on visor
-- LUC TO DO: Disabled, waiting for Tiago's procedural FX
--						if(not meleeType) then
--							meleeType = 1;
--						end
--						local effectId = MaterialEffects.GetEffectIdByName("player_fx", self.melee.effect[meleeType]);
--		  			if (effectId ~= MFX_INVALID_EFFECTID) then
--		    			spawned = MaterialEffects.ExecuteEffect(effectId, {pos=entity:GetWorldPos()});
--		  			end

					else
						hit.damage = melee.damage*(melee.damageSmall + (melee.damage - melee.damageSmall) * dot / 0.7) * self.melee.damageMultiplier ;
					end

				end

				hit.target	= entity;
				hit.targetId = entity.id;

				hit.normal = hit.normal or {};
	      CopyVector(hit.normal, hitDir);
	      NegVector(hit.normal);

				--g_gameRules.server:RequestMeleeHit(hit.shooter.id, hit.shooter.id, hit.target.id, 0, hit.pos);
				g_gameRules.Server.OnHit(g_gameRules, hit,false);
				g_gameRules.Client.OnHit(g_gameRules, hit,false);
	--				self:MeleeCustom(entity,pos,hitDir);

				-- TO DO: replace with Script bind actor.MeleeEffect() (using material)
				-- for all targets
				if(entity == g_localActor) then
					self:PlaySoundEvent("sounds/physics:bullet_impact:mat_armor_fp", g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
				end

        if(entity.actor and entity.actor:IsPlayer()) then
					local targetPos = g_Vectors.temp_v2;
					entity:GetPos(targetPos);
--					local distance = math.sqrt(distance2);
					entity:AddImpulse(-1,targetPos,impulse,300+(radius - distance)*100,1);

					local dotSide = dotproduct2d(impulse, entity:GetDirectionVector(0));
					local angImp = g_Vectors.temp_v3;
					angImp.x = randomF(-0.3,-0.2);
					angImp.y = 0;
					angImp.z = -dotSide * math.pi * (0.35 + 0.1*distance/radius);
					entity.actor:AddAngularImpulse(angImp, 0.0, 0.4);

					entity.actor:CameraShake(45, 0.3, 0.13, g_Vectors.v000);

					local energy = entity.actor:GetNanoSuitEnergy();
	        if (energy ~= 0) then
	          entity.actor:SetNanoSuitEnergy(energy-0.2*NANOSUIT_ENERGY);
	        end
				elseif(entity.vehicle and entity:IsEntityOnVehicle(g_localActor.id)) then
					-- player is on the meleed vehicle
					-- TO DO: improve with linear shake rather than angular
					g_localActor.actor:CameraShake(random(20,30), 0.2, 0.13, g_Vectors.v000);
			  end

				--AI.Signal(SIGNALFILTER_SENDER,0,"END_MELEE",self.id);
				--if(AI.IsEnabled(self.id)) then
					self:SelectPipe(0,"tr_end_melee");
				--end
				--Log("-------- melee end --------");
			end

		end
--	else
--		AI.LogEvent( "BasicAlien:MeleeDamage: no entities on radius ");
	end
end

function Trooper_x.Client:OnTimer(timerId,mSec)
	if (timerId == PAIN_TIMER) then
		if (self.actor:GetHealth() > 0) then
			self:DoPainSounds();
		end
		self.painSoundTriggered = nil;
	elseif (timerId == TROOPER_JUMP_TIMER) then
		Trooper_PerformSecondMeleeJump(self);
--	elseif (timerId == TROOPER_JUMP_CHASE_TIMER) then
--		local targetPos;
--		local vel;
--		local t;
--		local target = AI.GetAttentionTargetEntity(self.id);
--		local point = g_Vectors.temp_v1;
--		if(target) then
--			vel = g_Vectors.temp;
--			targetPos = target:GetPos();
--			t = AI.CanJumpToPoint(self.id,target.id,1,30,AI_JUMP_ON_GROUND,vel);
--			if(t) then
--				local pathFollower = target;
--				local parent = target:GetParent();
--				if(parent) then
--					pathFollower = parent;
--				end
--				if(not AI.GetPredictedPosAlongPath(pathFollower.id,t*1.0,point)) then
--					return;
--				end
--				if(parent) then
--					local disp = g_Vectors.temp_v2;
--					FastDifferenceVectors(disp,target:GetPos(),parent:GetPos());
--					FastSumVectors(point,point,disp);
--				end
--				point.z = point.z+1.5;
--				AI.SetRefPointPosition(self.id,point);
--				t = AI.CanJumpToPoint(self.id,point,1,30,AI_JUMP_ON_GROUND+AI_JUMP_CHECK_COLLISION,vel);
--			end
--		else
--			return;
--		end
--
--		local pos = g_Vectors.temp_v1;
--		local dir = g_Vectors.temp_v3;
--		CopyVector(pos,self:GetPos());
--		CopyVector(dir,self:GetDirectionVector(1));
--		FastDifferenceVectors(pos,pos,dir);
--		pos.z = pos.z + 2;
--		Particle.SpawnEffect("alien_special.Trooper.doubleJumpAttack", pos, dir, 0.5);
--    --self:PlaySoundEvent("Sounds/alien:trooper:jump_melee",g_Vectors.v000, g_Vectors.v010, sndFlags, SOUND_SEMANTIC_LIVING_ENTITY);
--
--		if(t) then
--			self.actor:SetParams({jumpTo = point, jumpVelocity = vel, jumpTime = t});
--		else
--			local entityAI = self.AI;
--			self.actor:SetParams({jumpTo = entityAI.jumpPos, jumpVelocity = entityAI.jumpVel, jumpTime = entityAI.jumpTime});
--		end
--		self:InsertSubpipe(AIGOALPIPE_NOTDUPLICATE+AIGOALPIPE_DONT_RESET_AG,"tr_jump_melee");
	elseif(timerId == TROOPER_END_JUMP_DODGE_TIMER) then
		local r = g_Vectors.temp;
		CopyVector(r,AI.GetRefPointPosition(self.id));
		Trooper_Jump(self,r,false,false,-15,true);
	elseif(timerId == TROOPER_DEPHYSICALIZE_TIMER) then
		if(self:GetSpeed()<0.3) then
			if(self.Properties.bIndoor==1 or AI.GetNavigationType(self.id) == NAV_WAYPOINT_HUMAN ) then
				self.actor:SetPhysicalizationProfile("unragdoll");
			end
		else
			self:SetTimer(TROOPER_DEPHYSICALIZE_TIMER,2000);
		end
	elseif(timerId == TROOPER_MELEE_SPECIAL_TIMER) then
		AI.Signal(SIGNALFILTER_SENDER,0,"MELEE_SPECIAL_START_TIMEOUT",self.id);
	elseif(timerId == TROOPER_END_MELEE_TIMER) then
		self:SetGrabbable(1);
		--self:SelectPipe(0,"tr_melee_special_timeout");
	elseif(timerId == TROOPER_CONVERSATION_REQUEST_TIMER) then
		AI.Signal(SIGNALFILTER_GROUPONLY_EXCEPT,0,"REQUEST_CONVERSATION",self.id);
		self:SetTimer(TROOPER_CONVERSATION_CHECK_TIMER,1000);
	elseif(timerId == TROOPER_CONVERSATION_CHECK_TIMER) then
		if(AIBlackBoard.trooper_ConversationState == TROOPER_CONV_REQUESTING) then
			-- still requesting, no answer - reset
			AIBlackBoard.trooper_ConversationState = TROOPER_CONV_IDLE;
			AI.Signal(SIGNALFILTER_SENDER,0,"REQUEST_CONVERSATION",self.id);
		end
	elseif(timerId == TROOPER_CONVERSATION_ANSWER_TIMER) then
		AI.Signal(SIGNALFILTER_SENDER,0,"CONVERSATION_ANSWER",self.id);
	elseif(timerId == TROOPER_PLAYERGRABBED_TIMER) then
		local sndFlags = SOUND_DEFAULT_3D;
    self.grabbedSound = self:PlaySoundEvent("sounds/alien:trooper:choke",g_Vectors.v000, g_Vectors.v010, sndFlags, SOUND_SEMANTIC_LIVING_ENTITY);
		self:SetTimer(TROOPER_PLAYERGRABBED_TIMER,5000+random(1,1000));
	elseif(timerId == TROOPER_CHECK_DEAD_SHELL_TIMER) then
		if(not AIBlackBoard.lastTrooperDisappearTime) then
			AIBlackBoard.lastTrooperDisappearTime = 0;
		end
		local curTime = _time;
		if(curTime - AIBlackBoard.lastTrooperDisappearTime > 2) then
			local playerViewDir = TrTimerVector1;
			local dir = TrTimerVector2;
			g_localActor.actor:GetHeadDir(playerViewDir);
			FastDifferenceVectors(dir,self:GetPos(),g_localActor:GetPos());
			local dist = LengthVector(dir);
			if(dist>2) then
				ScaleVectorInPlace(dir,1/dist);
				if(dotproduct3d(playerViewDir,dir) < 0.2) then
					-- make the trooper magically disappear out of player's sight
					AIBlackBoard.lastTrooperDisappearTime = curTime;
					AI.SetSmartObjectState(self.id,"Idle");
					self:RemoveActor();
					return;
				end
			end
		end
		self:SetTimer(TROOPER_CHECK_DEAD_SHELL_TIMER,2000);
	elseif(timerId == TROOPER_WARMUP_AUTODESTRUCT_TIMER) then
		self:WarmupAutoDestruct();
	elseif(timerId == TROOPER_AUTODESTRUCT_TIMER) then
		self:AutoDestruct();
	elseif(timerId == TROOPER_GRABBEDFX_TIMER) then
		self:ResetAttachment(0, "Grapped");
		self.AI.bGrabbedFx = false;
	end
end

function Trooper_x:SetFireMode()
	local item = self.inventory:GetCurrentItem();
	if (item and item.weapon) then
		if(self.AI.FireMode ==1) then
			item.weapon:SetCurrentFireMode("Automatic");
		else
			item.weapon:SetCurrentFireMode("Rapid");
		end
	else
		--System.Warning("Entity "..self:GetName().." has no item/weapon");
	end

end


function Trooper_x:OnLoad(saved)

--	BasicAI.OnLoad(self,saved);
	if( not AIBlackBoard.Trooper_SpecialActionTarget) then
		AIBlackBoard.Trooper_SpecialActionTarget = {};
	end
	local satid = saved.MySpecialActionTargetId;
	if(satid) then
		AIBlackBoard.Trooper_SpecialActionTarget[self:GetName()] = System.GetEntity(satid);
	else
		AIBlackBoard.Trooper_SpecialActionTarget[self:GetName()] = nil;
	end
	self.speedSoundScaleGoal = saved.speedSoundScaleGoal;
	self.speedSoundScale = saved.speedSoundScale;
	self.newSpeedSoundScaleGoal = saved.newSpeedSoundScaleGoal;
	self.bAutoDestructing = saved.bAutoDestructing;
	self.exploded = saved.exploded;

end

function Trooper_x:OnLoadAI(saved)
	BasicAI.OnLoadAI(self,saved);
	if(self.AI.bGrabbedFx) then
		self:SetAttachmentEffect(0, "Grapped", "alien_special.Trooper.carried", g_Vectors.v000, g_Vectors.v010, 1, 0);
	end
end

function Trooper_x:OnSave(save)
--	BasicAI.OnSave(self,save);

	if(AIBlackBoard.Trooper_SpecialActionTarget) then
		for name,tg in pairs(AIBlackBoard.Trooper_SpecialActionTarget) do
			if(name == self:GetName()) then
				save.MySpecialActionTargetId = tg.id;
			end
		end
	end
	save.speedSoundScaleGoal = self.speedSoundScaleGoal;
	save.speedSoundScale = self.speedSoundScale;
	save.newSpeedSoundScaleGoal = self.newSpeedSoundScaleGoal;
	save.bAutoDestructing = self.bAutoDestructing;
	save.exploded = self.exploded;

end

function Trooper_x:SetGrabbable(value)
	self.Properties.Damage.bNoGrab = 1 - (value or 0);
end

function Trooper_x:Beam(target)

	if(target) then
--		System.Log(self:GetName().." FIRING BEAM at "..target:GetName());
		self.actor:CreateCodeEvent({event="beamStart",effect="Alien_Weapons.Freeze_Beam.Trooper_MOAR_firing",targetId=target.id});
	else
--		System.Log(self:GetName().." STOPPING BEAM");
		self.actor:CreateCodeEvent({event="beamStop"});
	end
end

function Trooper_x:RegisterWithExplosion(entity)
	if(not self.RegisteredWithExplosion) then
		self.RegisteredWithExplosion = {};
	end
	AI.LogEvent("REGISTERING "..entity:GetName().." WITH EXPLOSION");
	self.RegisteredWithExplosion[entity]=entity;
end

function Trooper_x:NotifyExplosion()
	if(self.RegisteredWithExplosion) then
		for i,entity in pairs(self.RegisteredWithExplosion) do
			AI.LogEvent("NOTIFYING "..entity:GetName().." WITH EXPLOSION");
			AI.Signal(SIGNALFILTER_SENDER,10,"OnExplosion",entity.id);
		end
	end
end

-----------------------------------------------------------------------------------------------------
function Trooper_x:GetDamageImpulseMultiplier(hit)
	if(self.AI.noDamageImpulse) then
		return 0;
	end
	local mult = 1;
	if(hit.weapon and hit.weapon.class and hit.weapon.class ~="") then
		mult = self.Properties.Damage.ImpulseMultipliers[hit.weapon.class];
		if(not mult) then
			self.Properties.Damage.ImpulseMultipliers[hit.weapon.class] = 1;
			mult = 1;
		end
	end
	if(self.actor:IsFlying()) then
		mult = mult*0.75;
	end
  return mult;
end


function Trooper_x:Event_Jump(params,pos)
	Trooper_Jump(self,pos,true,true);
	self.actor:SetParams({useLandEvent = true});
end

function Trooper_x:SetJumpSpecialAnim(animType,inputType,inputValue)
	self.actor:SetParams({specialAnimType = animType, specialAnimAGInput = AGInputType, specialAnimAGInputValue = inputValue});
end


function Trooper_x:Event_EnableCloaked(params)
	self:Event_Enable(params);
	self:Event_Cloak();
end

--function Trooper_x:Event_EnablePhysics(params)
--	self.actor:SetPhysicalizationProfile("ragdoll");
--end
--
--function Trooper_x:Event_DisablePhysics(params)
--	self.actor:SetPhysicalizationProfile("unragdoll");
--end


function Trooper_x:AnimationEvent(event,value)
	if ( event == "Jump" ) then
		if(self.AI.doubleJump) then
			self:SetTimer(TROOPER_JUMP_TIMER,self.AI.firstJumpTime*1000);
			self.AI.doubleJump = false;
		end
	elseif ( event == "JumpMeleeGround" ) then
		local target = AI.GetAttentionTargetEntity(self.id);
		if (self.actor:GetHealth()<=0) then
			return
		end
		local targetVel = TrVector_v0;
		local targetPos = TrVector_v1;
		local t;
		local vel = g_Vectors.temp;
		local pos = g_Vectors.temp_v1;
		local dir = g_Vectors.temp_v2;
		CopyVector(pos,self:GetPos());
		if(target) then
			target:GetVelocity(targetVel);
			CopyVector(targetPos, target:GetPos());
			-- approximate prediction of 1 second
			FastSumVectors(targetPos,targetPos,targetVel);
			FastDifferenceVectors(dir,pos,targetPos);
			dir.z=0;
			NormalizeVector(dir);
			FastSumVectors(targetPos,targetPos,dir);
			local angle, dist = Trooper_GetJumpAngleDist(self,targetPos,0,60);
			if(angle) then
				t = AI.CanJumpToPoint(self.id,targetPos,angle,20,AI_JUMP_ON_GROUND + AI_JUMP_CHECK_COLLISION,vel);
			end
		end

		CopyVector(dir,self:GetDirectionVector(1));
		FastDifferenceVectors(pos,pos,dir);
		pos.z = pos.z + 1.2;
		Particle.SpawnEffect("alien_special.Trooper.doubleJumpAttack", pos, dir, 0.5);
    self:PlaySoundEvent("Sounds/alien:trooper:jump_burst",g_Vectors.v000, dir, SOUND_DEFAULT_3D, SOUND_SEMANTIC_LIVING_ENTITY);

		if(t) then
			self.actor:SetParams({jumpTo = targetPos, jumpVelocity = vel, jumpTime = t});
			AI.SetRefPointPosition(self.id,targetPos);
		else
			local entityAI = self.AI;
			self.actor:SetParams({jumpTo = entityAI.jumpPos, jumpVelocity = entityAI.jumpVel, jumpTime = entityAI.jumpTime});
		end
	elseif ( event == "jumpToSOExitPoint" ) then
		local exitPos = g_Vectors.temp;
		if(AI.GetLastSmartObjectExitPoint( self.id,exitPos )) then
			Trooper_Jump(exitPos);
		end
	elseif( event == "MeleeDamage" ) then
		local impulse = g_Vectors.temp;
		self:GetMeleeDamageImpulse(impulse,value);
		self:MeleeDamage(impulse,value);
	elseif( event == "EngineSpeed") then
		if(value == nil or value==0) then
			self.animationEngineSpeed = false;
		else
			self.animationEngineSpeed = true;
			self.speedSoundScaleGoal = value;
		end
	else
		BasicAI.AnimationEvent(self,event,value);
	end

end


Trooper_x.FlowEvents =
{
	Inputs =
	{
		EnableCloaked = { Trooper_x.Event_EnableCloaked, "bool" },
		Cloak = { BasicAlien.Event_Cloak, "bool" },
		JumpAt = { Trooper_x.Event_Jump, "vec3" },
--		EnablePhysics = { Trooper_x.Event_EnablePhysics, "bool" },
--		DisablePhysics = { Trooper_x.Event_DisablePhysics, "bool" },
	},
	Outputs =
	{
		EnableCloaked = "bool",
		Cloak = "bool",
		Land = "bool",
--		EnablePhysics = "bool",
--		DisablePhysics = "bool",
	},
}
