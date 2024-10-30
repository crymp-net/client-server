Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicAlien.lua");

Alien_x = {

	foreignCollisionDamageMult = 0.1,
	selfCollisionDamageMult = 0.01,
	collisionKillVelocity = 20.0,

	isAlien = 1,
	isBlind = false,

	voiceType = "alienTest",
------------------------------------------------------------------------------------

	PropertiesInstance = {
		aibehavior_behaviour = "GuardIdle",
		bAutoDisable = 1,
	},

	Properties = {
	  bExactPos = 0,
		attackrange = 70,
		accuracy = 1.0,
		species = 2,
		aicharacter_character = "GuardAssault",
		soclasses_SmartObjectClass = "Alien",
		--fileModel = "Objects/characters/alien/methagen/alien.chr",
		--fileModel = "Objects/characters/alien/methagen/MethagenWBJuly05/MGuard.chr",
		fileModel = "Objects/characters/alien/alienbase/alienBase.cdf",
		damageScale = 1,
		SoundPack = "Alien",
		AnimPack = "Alien",
		voiceType = "alien",

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
			FOVPrimary = 80,			-- normal fov
			FOVSecondary = 160,		-- periferial vision fov
			--ranges
			sightrange = 50,
			--how heights of the target affects visibility
			--// compare against viewer height
			-- fNewIncrease *= targetHeight/stanceScale
			stanceScale = 1.8,
			-- Equivalent to camo scale, used with thermal vision.
			heatScale = 1.0,
			-- Flag indicating that the agent has thermal vision.
			bThermalVision = 1,
			-- The perception reaction speed, default speed = 1. THe higher the value the faster the AI acquires target.
			reactionSpeed = 1.5,
		},
		Behavior =
		{
			-- These parameters are shared between the initial behavior.
			targetRange = 18.0,			-- The preferred range for: Attack, Defend, Hide, or Evade.
			targetEntity = "",			-- The preferred location for: Attack, Defend, Hide, or Evade.
			alarmLevel = 1,					-- 0=never alarm, 1=alarm when half of group is killed, 2=alarm when one of the group is killed, 3=alarm on enemy contact
			cloakLevel = 0,					-- 0=No cloaking, 1=Cloak during Evade/Hide, 2=Cloak during Evade/Hide and when hiding.
		},
		Damage =
		{
			health = 600,
		},
	},

	physicsParams =
	{
		flags = 0,

		-- + pef_ignore_areas,
		Living =
		{
			air_resistance = 0.0,
		},
	},

	gameParams =
	{
		stance =
		{
			{
				stanceId = STANCE_STAND,
				normalSpeed = 6.5,
				maxSpeed = 13.0,
				heightCollider = 0.0,
				heightPivot = -0.1,
				size = {x=0.6,y=0.6,z=0.3},
				viewOffset = {x=0,y=0.35,z=0.75},
				weaponOffset = {x=0.45,y=0,z=-0.70},
				name = "vertRelaxed",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_STEALTH,
				normalSpeed = 6.5,
				maxSpeed = 13.0,
				heightCollider = 0.0,
				heightPivot = -0.1,
				size = {x=0.6,y=0.6,z=0.3},
				viewOffset = {x=0,y=0,z=0},
				weaponOffset = {x=0,y=0,z=0},
				name = "vertAlerted",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_PRONE,
				normalSpeed = 10.0,
				maxSpeed = 18.0,
--				heightCollider = 1.0,
--				heightPivot = 0.9,
--				size = {x=0.5,y=0.5,z=0.3},
--				viewOffset = {x=0,y=0.15,z=0.625},
--				modelOffset = {x=0,y=0,z=-1.0},
				heightCollider = 0.0,
				heightPivot = 0,
				size = {x=0.6,y=0.6,z=0.01},
				viewOffset = {x=0,y=0.90,z=0.10},
				weaponOffset = {x=0.8,y=0.30,z=-0.25},
				name = "prone",
				useCapsule = 1,
			},
		},

		speedInertia = 6.0,--the more, the faster the speed change: 1 is very slow, 10 is very fast already
		rollAmount = 3.5, --3.0,
		rollSpeed = 0.7,

		sprintMultiplier = 1.5,--speed is multiplied by this ammount when alien is sprinting
		sprintDuration = 0.2,--how long the sprint is

		rotSpeed_min = 12.0,--rotation speed at min speed
		rotSpeed_max = 6.0,--rotation speed at max speed

		speed_min = 1.0,--used by the above parameters

		forceView = 2.0,--multiply how much the view is taking into account when moving

		tentaclesRadius = 0.07,

		trailEffect = "alien_special.alien_movement_trail.alien_refraction",
		trailEffectMinSpeed = 1,
		trailEffectMaxSpeedSize = 0,
		trailEffectMaxSpeedCount = 3.0,
		trailEffectDir = {x=-1,y=0,z=0},

--		healthTrailEffect = "alien_special.alien_movement_trail.alien_blood_trail",
--		healthTrailEffectMaxSize = 0,
--		healthTrailEffectMaxCount = 1.0,
--		healthTrailEffectDir = {x=-1,y=0,z=0},
	},

	--melee stuff
	melee =
	{
		melee_animations =
		{
			{
				"alien_melee_01",
--				"melee_04",
				1,
			},
		},

		damage = 300 / 0.2,			-- damage when doing melee from front
		damageSmall = 160 / 0.2,	-- damage when doing melee from back
		damageOffset = {x=0,y=0,z=0};		-- Local offset of the damage box
		damageRadius = 2.5,								-- size of the damage box.
		approachLookat = 0, --1,
		alignTime = 2.4, --1.45,
		damageTime = 3.3, --2.1,
	},

	grabParams =
	{
		collisionFlags = geom_colltype_player,
		holdPos = {x=0,y=-2.5,z=-0.75}, -- default position where the grab is holded.
	},

	-- the AI movement ability of the vehicle.
	AIMovementAbility =
	{
		usePredictiveFollowing = 1,
		pathRadius = 1.7,
		pathLookAhead = 2, --5,
		walkSpeed = 6.5,
		runSpeed = 10.0,
		sprintSpeed = 14.0,
		maneuverSpeed = 7.0,
		b3DMove = 1,
		minTurnRadius = 0.1,		-- meters
		maxTurnRadius = 4.0,		-- meters
		pathSpeedLookAheadPerSpeed = 0.1,
		cornerSlowDown = 0,
		pathType = AIPATH_3D,
		passRadius = 0.6,		-- the radius used in path finder.
		pathFindPrediction = 0.05,		-- predict the start of the path finding in the future to prevent turning back when tracing the path.
	},

	_TempPhysParams = {},

	Client = {};
	Server = {};

	lastSpeed = -1,
}

ALIEN_AFTER_DEAD_GLOW_TIMER = 200;

function Alien_x:CreateAttachments()
	-- create attachment points
	self:CreateBoneAttachment(0, "R_finger03_end", "right_item_attachment");
	self:CreateBoneAttachment(0, "spine01", "trail_attachment");
	self:CreateBoneAttachment(0, "spine01", "health_trail_attachment");
end

function Alien_x:OnResetCustom()

	self.bleeding = 0.0;
	self.nextBleed = 0.0;
	self.lastSpeed = -1;
	--self:ResetMaterial(-1);

end

function Alien_x.Client:OnUpdate(frameTime)

	if (self.bleeding) then
		self.bleeding = self.bleeding - frameTime;

		self:Bleed(frameTime);

		if (self.bleeding < 0.0) then
			self.bleeding = nil;
		end
	end

	if (self.idleSound) then
		local speed = self:GetSpeed();
		if (speed ~= self.lastSpeed) then
			local val = __min(1.0, speed / 13.0);
			Sound.SetParameterValue(self.idleSound, "rpm_scale", val);
		end
		self.lastSpeed = speed;
	end

end

function Alien_x.Client:OnTimer(timerId,mSec)
	if (timerId == ALIEN_AFTER_DEAD_GLOW_TIMER) then
		local dt_fadeout = (mSec/1000)*10;
		self.mtlBodyGlow = self.mtlBodyGlow - dt_fadeout*self.mtlGlowDir;
		self.mtlHeadGlow = self.mtlHeadGlow - dt_fadeout*self.mtlGlowDir;
		if (self.mtlHeadGlow > 3) then
			self.mtlGlowDir = 1;
		end
		if (self.mtlHeadGlow > 0) then
			self:SetMaterialFloat( 0,0,"glow",self.mtlBodyGlow );
			self:SetMaterialFloat( 0,1,"glow",self.mtlHeadGlow );
			self:SetTimer(ALIEN_AFTER_DEAD_GLOW_TIMER,mSec);
		end
	else
		BasicActor.Client.OnTimer(self,timerId,mSec);
	end
end


function Alien_x:Bleed(frameTime)

	self.nextBleed = self.nextBleed - frameTime;

	if (self.nextBleed < 0.0) then
		local pos = g_Vectors.temp_v1;

		-- Using the world pos for testing purposes! [Mikko]
		CopyVector(pos,self:GetWorldPos());

--		CopyVector(pos,self:GetBonePos("root"));

		Particle.SpawnEffect("explosions.AlienBlood.b", pos, g_Vectors.v001, 1.0);

		self.nextBleed = randomF(0.05,0.15);
	end
end

function Alien_x:Kill(ragdoll, shooterId, weaponId)

	-- If carrying object, remove 'Being_Carried' from the SO states before dropping it.
	local wasCarrying = false;
	if (self.grabParams.entityId) then
		wasCarrying = true;
		AI.ModifySmartObjectStates(self.grabParams.entityId, "-Being_Carried");
	end

	BasicAlien.Kill(self,ragdoll,shooterId,weaponId);

	--gibbing
	if (self.lastHit.explosion) then
		Particle.SpawnEffect("alien_special.AlienBlood.alien_explode", self:GetWorldPos(), g_Vectors.v001, 1.0);
		self:RemoveActor();
		return;
	end

	if (ragdoll) then
		self.actor:SetPhysicalizationProfile("ragdoll");
	end

	self.bleeding = 10.0;
	self.nextBleed = 0.0;

	------------------------------------------------------------------
	-- Change dumping of the ragdoll
	------------------------------------------------------------------
	self._TempPhysParams.damping = 0.5;
	self._TempPhysParams.freefall_damping = 0.5;
	self:SetPhysicParams( PHYSICPARAM_SIMULATION,self._TempPhysParams );
	------------------------------------------------------------------

	------------------------------------------------------------------

------------------------------------------------------------------
	local lastVelocity = self.lastHit.velocity;
	local lastSpeed = LengthVector(lastVelocity);

	local dir = g_Vectors.temp_v2;

	if (self.lastHit.dir) then
		CopyVector(dir, self.lastHit.dir);
	else
		dir.x = randomF(-1.0,1.0);
		dir.y = randomF(-1.0,1.0);
		dir.z = randomF(-1.0,1.0);
	end

	FastSumVectors(dir, dir, lastVelocity);

	local impulse = 5000 * randomF(1.0,1.5);

	-- If the alien was carrying something, decrease the impulse so that the carried stuff
	-- does not break or fly to the moon.
	if (wasCarrying) then
		impulse = impulse * 0.1;
	end

	local ppos = g_Vectors.temp_v3;
	local partId = self.lastHit.partId;

	if (self.lastHit.pos) then
		CopyVector(ppos,self.lastHit.pos);
	else
		self:GetWorldPos(ppos);
	end

	if (not partId) then
		partId = -1;
	end

	--self.Log( self.dds );

	self:AddImpulse( partId, ppos, dir, impulse,1 );
------------------------------------------------------------------

	-- Clone material to make it unique.
	self:CloneMaterial(0);
	self.mtlGlowDir = -2;
	self.mtlBodyGlow = self:GetMaterialFloat( 0,0,"glow" );
	self.mtlHeadGlow = self:GetMaterialFloat( 0,1,"glow" );

	-- Start glow fading out on his new cloned material.
	self:SetTimer(ALIEN_AFTER_DEAD_GLOW_TIMER,1);


	------------------------------------------------------------------
	-- Load dead particles
	------------------------------------------------------------------
	local params =
	{
		SpawnPeriod			= 0,
		Scale						= 3,
		CountScale			= 1,
		bCountPerUnit		= 0,
		bSizePerUnit		= 0,
		AttachType			= "none",
		AttachForm			= "none",
		bPrime					= 0,
	}
	self:LoadParticleEffect( -1, "alien_special.Alienblood.dead_alien", params );
	------------------------------------------------------------------

	-- Alien Blood Splat on player (player is close enough and roughly faces alien)
	--self:MakeBloodSplats("BloodSplats_Alien", 9, ppos);

	--explosion
--	local pos = self:GetWorldPos();
--	Particle.SpawnEffect("alien_special.scout.ScoutExplosion", pos, g_Vectors.v001, 0.35);
--	g_gameRules:ClientViewShake(pos,10,40,0.25,0.09);
--
--	self:RemoveActor();
end


function Alien_x.Client:OnHit(hit, remote)
	if (not self.bleeding) then
		self.bleeding = 1.0;
		self.nextBleed = 0.0;
	end

	if (string.find(hit.type,"bullet")) then
		if (self.nextBloodsplat == nil or self.nextBloodsplat < _time) then
			self.nextBloodsplat = _time + random(2, 4);
			self:MakeBloodSplats("BloodSplats_Alien", 9, hit.pos);
		end
	end

	if (not BasicAlien.Client.OnHit(self,hit,remote)) then
			-- Apply more impulse on hit
		self:AddImpulse(hit.partId,hit.pos,hit.dir,randomF(1000,5000),1);
		return false;
	end

	-- Apply more impulse on hit
	self:AddImpulse(hit.partId,hit.pos,hit.dir,randomF(150,350),1);

	-- Set to play pain animation.
	self.actor:SetAnimationInput( "SignalReaction", "pain" );

	--FIXME: not hardcoded like this
--		if( hit.damage > 90 ) then
--		self:StartAnimation(0, "alien_hitProne_front_01", 3, 0.1, 1.0, false,true);
--		else
--			self:StartAnimation(0, "receive_damage_small_front_01", 3, 0.1, 1.0, false,true);
--		end

	if(hit.shooter~=nil) then
		g_SignalData.id = hit.shooter.id;
		g_SignalData.fValue = hit.damage;
		CopyVector(g_SignalData.point, hit.shooter:GetWorldPos());
		if (self.Properties.species ~= hit.shooter.Properties.species) then
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
		g_SignalData.fValue = damage;
		CopyVector(g_SignalData.point, g_Vectors.v000);
		AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
	end
end

--------------------------------------------------------------------------------------------------------
function Alien_x:OnAGSlowSpeed()
	AI.LogComment("BasicAI:OnAGSlowSpeed()");
--	self.temp_stats.idealSpeed = 0.8;
--	self.actor:SetParams(self.temp_stats);
end

--------------------------------------------------------------------------------------------------------
function Alien_x:OnAGNormalSpeed()
	AI.LogComment("BasicAI:OnAGNormalSpeed()");
--	self.temp_stats.idealSpeed = -1;
--	self.actor:SetParams(self.temp_stats);
end

--------------------------------------------------------------------------------------------------------
function Alien_x:AlienMakeIdle()
	-- Make this guy idle
	AI.ChangeParameter(self.id,AIPARAM_SIGHTRANGE,self.Properties.Perception.sightrange);
	AI.ChangeParameter(self.id,AIPARAM_FOVPRIMARY,self.Properties.Perception.FOVPrimary);
end

--------------------------------------------------------------------------------------------------------
function Alien_x:AlienMakeAlerted( noDrawWeapon )

--	do return end

	self:StopConversation();
	-- Make this guy alerted
	AI.ChangeParameter(self.id,AIPARAM_SIGHTRANGE,self.Properties.Perception.sightrange*1.5);
	if(self.Properties.Perception.FOVPrimary>0) then
		AI.ChangeParameter(self.id,AIPARAM_FOVPRIMARY,self.Properties.Perception.FOVSecondary);		-- focus attention
	end

	if(noDrawWeapon~=nil) then return end

	if(not self.inventory:GetCurrentItemId()) then
		self:HolsterItem(false);
	end

	-- lets set burst fire mode - only Kuang has it currently
	local weapon = self.inventory:GetCurrentItem();
	if(weapon~=nil and weapon.class=="Kuang") then
		weapon.weapon:SetCurrentFireMode("burst");
	end
end

Alien_x.FlowEvents =
{
	Inputs =
	{
	},
	Outputs =
	{
	},
}

function Alien_x:MakeParalyzed( dir, strength )

	AIBehaviour.GUARDDEFAULT:GotoPursue(self,1);
	self:InsertSubpipe(0,"gr_paralyze");
	self.actor:SetPhysicalizationProfile("ragdoll");

	local stats = self:GetPhysicalStats();
	self:AddImpulse(-1, pos, dir, stats.mass*10, 1);

	Script.SetTimerForFunction(15 * 1000,"AlienUnmakeParalyzed",self);
end

function AlienUnmakeParalyzed(entity,timerId)
	entity:Physicalize(0, PE_LIVING, entity.physicsParams);
	entity.actor:PostPhysicalize();
	entity.actor:Revive();
end
-----------------------------------------------------------------------------------------------------

function metatest()
	local a = System.GetEntityByName("Alien1");
	local b = System.GetEntityByName("Target1");
	local c = System.GetEntityByName("Target2");

	--a.actor:SetMovementTarget(b:GetWorldPos(),c:GetWorldPos());
	a.actor:SetMovementTarget(b:GetWorldPos(),{x=0,y=0,z=0},{x=0,y=0,z=0},1);
	--a.actor:SetMovementTarget({x=0,y=0,z=0},c:GetWorldPos());
end

function ReviveAliens()
	local entities = System.GetEntitiesByClass("Alien");

	if (entities) then
		for i,entity in pairs(entities) do
			if (entity and entity.OnReset) then
				entity:OnReset();
			end
		end
	end
end

function Alien_x:OnSave(save)
	save.lastSpeed = self.lastSpeed;
	save.mtlGlowDir = self.mtlGlowDir;
	save.mtlBodyGlow = self.mtlBodyGlow;
	save.mtlHeadGlow = self.mtlHeadGlow;
end

function Alien_x:OnLoad(save)
	self.lastSpeed = save.lastSpeed;
	self.mtlGlowDir = save.mtlGlowDir;
	self.mtlBodyGlow = save.mtlBodyGlow;
	self.mtlHeadGlow = save.mtlHeadGlow;
end
