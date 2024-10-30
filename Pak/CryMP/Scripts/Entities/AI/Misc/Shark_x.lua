--------------------------------------------------------------------------
--	Crytek Source File.
-- 	Copyright (C), Crytek Studios, 2001-2006.
--------------------------------------------------------------------------
--	$Id$
--	$DateTime$
--	Description: Shark class to be used for kill events
--
--------------------------------------------------------------------------
--  History:
--  - 6/2006     : Created by Sascha Gundlach
--
--------------------------------------------------------------------------
Script.ReloadScript("Scripts/Entities/AI/Shared/BasicAI.lua");
Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicActor.lua");

SHARK_SINK_TIMER = 1;
SHARK_DELETE_TIMER = 2;
SHARK_IMPULSE_TIMER = 3;

Shark_x = {
--	Client = {},
--	Server = {},
	AnimationGraph = "Shark.xml",
	--AnimationGraph = "",
	idleAnimation = "shark_swim_01",
	UpperBodyGraph = "",

	Properties = {
		fileModel 					= "Objects/characters/animals/whiteshark/greatwhiteshark.cdf",
		lying_gravityz = -5.0,
    lying_damping = 1.5,
    bCollidesWithPlayers = 1,
    bPushableByPlayers = 0,
    bInvulnerable =0,
    bCollisionInvulnerable = 1,
    Mass = 400,
		aicharacter_character = "SuperDumb",
 		soclasses_SmartObjectClass = "Shark",
 		voiceType = "none",
 		bSpeciesHostility = 1,
 		species = 1,
		SoundPack = "",
		Damage = {
			health = 3000,
			FallPercentage = 0,
			bNoGrab = 1,
		},

	},

	PropertiesInstance = {
		bAutoDisable = 0,
		groupid = 174,
		aibehavior_behaviour = "SuperDumbIdle",
	},

	Physics = {
		bRigidBody=1,
		bRigidBodyActive = 0,
		bRigidBodyAfterDeath = 0,
		bActivateOnDamage = 0,
		bResting = 1,
		Density = 2,
		Mass = 400,
		Simulation =
		{
      sleep_speed = 0.04,
		  damping = 1,
      bFixedDamping = 0,
		  bUseSimpleSolver = 0,
		},
		Buoyancy=
		{
			water_density = 20,
			water_damping = 1,
			water_resistance = 40,
		},
	},
	DeadBodyParams = {
	  max_time_step = 0.025,
	  gravityz = -7.5,
	  sleep_speed = 0.025,
	  damping = 0.3,
	  freefall_gravityz = -9.81,
	  freefall_damping = 0.1,

	  lying_mode_ncolls = 4,
	  lying_gravityz = -5.0,
	  lying_sleep_speed = 0.065,
	  lying_damping = 1.5,
	  sim_type = 1,
		lying_simtype = 1,
	},

	PhysParams = {
		mass = 400,
		height = 0.0,
		eyeheight = 0.0,
		sphereheight = 0.0,
		radius = 0.0,
	},

	gameParams =
	{
		speedInertia = 6.0,--the more, the faster the speed change: 1 is very slow, 10 is very fast already
		rollAmount = 3.5, --3.0,
		rollSpeed = 0.7,

		sprintMultiplier = 1.5,--speed is multiplied by this ammount when alien is sprinting
		sprintDuration = 0.2,--how long the sprint is

		rotSpeed_min = 1.8,--rotation speed at max speed
		rotSpeed_max = 3.5,--rotation speed at min speed

		minTurnRadius = 3,

		speed_min = 2,--used by the above parameters

		forceView = 2.0,--multiply how much the view is taking into account when moving

		accel = 2.0;
		decel = 2.0;

		--trailEffect = "alien_special.alien_movement_trail.alien_refraction",
		trailEffectMinSpeed = 1,
		trailEffectMaxSpeedSize = 0,
		trailEffectMaxSpeedCount = 3.0,
		trailEffectDir = {x=-1,y=0,z=0},

		minDistanceCircle = 13,
		maxDistanceCircle = 21,
		circlingTime = 15,
		numCircles = 2,
		headOffset = 3,
		minDistForUpdatingMoveTarget = 4,
		headBone = "spine01",
		spineBone1 = "spine02",
		spineBone2 = "spine03",
		escapeAnchorType = AIAnchorTable.SHARK_ESCAPE_SPOT;

		stance =
		{
			{
				stanceId = STANCE_STAND,
				normalSpeed = 10.5,
				maxSpeed = 10.5,
				heightCollider = 0,
				heightPivot = 0.0,
				size = {x=0.6,y=0.6,z=0.4},
				viewOffset = {x=0,y=3.0,z=0.0},
				weaponOffset = {x=0.45,y=0,z=-0.0},
				name = "vert",
			},
			{
				stanceId = STANCE_RELAX,
				normalSpeed =10.5,
				maxSpeed = 10.5,
				heightCollider = 0.0,
				heightPivot = 0.0,
				size = {x=0.6,y=0.6,z=0.4},
				viewOffset = {x=0,y=3.0,z=0.0},
				weaponOffset = {x=0.45,y=0,z=-0.0},
				name = "vert",
			},
		},

		melee =
		{
			animation = "whiteshark_biteUpperBody_01",
			radius = 7,
			rollTime = 2;
			rollAngle = 45;
		},
	},

	SinkBuoyancy=
	{
		water_density = 30,
		water_damping = 0,
		water_resistance = 100,
	},

	pos = {x=0,y=0,z=0},
	vel = {x=0,y=0,z=0},
	fleeTime = 0,
	meleeTime = 0,
	goAwayTime = 0,
	dead = false,
	radius = 40,
	FXSlot = -1,

	AIMovementAbility =
	{
		usePredictiveFollowing = 1,
		pathRadius = 1.7,
		pathLookAhead = 2, --5,
		walkSpeed = 6.5,
		runSpeed = 10.0,
		sprintSpeed = 14.0,
		maneuverSpeed = 7.0,
		b3DMove = 0,
		minTurnRadius = 0.1,		-- meters
		maxTurnRadius = 4.0,		-- meters
		pathSpeedLookAheadPerSpeed = 0.1,
		cornerSlowDown = 0,
		pathType = AIPATH_DEFAULT,
		passRadius = 0.6,		-- the radius used in path finder.
		pathFindPrediction = 0.05,		-- predict the start of the path finding in the future to prevent turning back when tracing the path.
	},

--	Delete = function(self)
--		System.RemoveEntity(self.id);
--	end,


	Server = {},
	Client = {},


}


function Shark_x:OnPropertyChange()
	self:OnReset();
end;

function Shark_x:OnSave(tbl)
--	tbl.health=self.health;
--	tbl.pos=self.pos;
--	tbl.vel=self.vel;
--	tbl.fleeTime=self.fleeTime;
--	tbl.meleeTime=self.meleeTime;
--	tbl.goAwayTime=self.goAwayTime;
	tbl.dead=self.dead;
	tbl.FXSlot=self.FXSlot;
end;

function Shark_x:OnLoad(tbl)
--	self.health=tbl.health;
--	self.pos=tbl.pos;
--	self.vel=tbl.vel;
--	self.fleeTime=tbl.fleeTime;
--	self.meleeTime=tbl.meleeTime;
--	self.goAwayTime=tbl.goAwayTime;
	self.dead=tbl.dead;
	self.FXSlot=tbl.FXSlot;
end;

function Shark_x:OnReset()
	self:StopBleeding();
	if(not EmptyString(self.Properties.fileModel))then
		self:LoadObject( 0,self.Properties.fileModel );
	end
	self:PhysicalizeThis();
	self:EnablePhysics(true);
	self:AwakePhysics(1);
	self:Activate(0);

	self.dead = false;


	BasicActor.ResetCommon(self);
	self.AI.spawned = false;

end;

function Shark_x:PhysicalizeThis()
	local Physics=self.Physics;
	EntityCommon.PhysicalizeRigid(self,0,Physics,self.bRigidBodyActive );
  self:SetPhysicParams(PHYSICPARAM_SIMULATION, Physics.Simulation);
	self:SetPhysicParams(PHYSICPARAM_BUOYANCY, Physics.Buoyancy);
	local Flags = { flags_mask=pef_fixed_damping+ref_use_simple_solver, flags=0 };
  if Physics.Simulation.bFixedDamping then
		Flags.flags = Physics.Simulation.bFixedDamping*pef_fixed_damping;
	end
	if Physics.Simulation.bUseSimpleSolver then
		Flags.flags = Flags.flags+Physics.Simulation.bUseSimpleSolver*ref_use_simple_solver;
	end
	self:SetPhysicParams(PHYSICPARAM_FLAGS, Flags );
end;


function Shark_x:DoMeleeDamage()
		local target = self.AI.target;
		if(target and not target:IsDead() and not target:IsOnVehicle())then
			local targetpos=target:GetPos();
			local rnd=random(1,2);
			--self:StartAnimation(0, "shark_attack_0"..rnd, 0, 0.25, 1.0, false,true, true);
			g_gameRules:ClientViewShake(targetpos,10,50,0.5,0.09);
			g_gameRules:CreateHit(target.id,self.id,self.id,500,nil,nil,nil,"melee");
			Particle.SpawnEffect("misc.blood_fx.water_pulse",targetpos,g_Vectors.v001,3.0);
			local vel = g_Vectors.temp;
			self:GetVelocity(vel);
			local mass = target:GetMass();
			target:AddImpulse(-1,nil,vel,mass * LengthVector(vel),1);

--		else
--			--self:StartAnimation(0, "whiteshark_swimFast_forward_01", 0, 0.25, 1.0, true,true, true);
--			self.goAwayTime=4.0;
		end;
end;

function Shark_x:AnimationEvent(event,value)
	if ( event == "meleeDamage" ) then
		self:DoMeleeDamage();
	end

end
----------------------------------------------------------------------------------------------------
--function Shark_x.Server:OnInit()
--	if(not self.bInitialized)then
--		self:OnReset();
--		self.bInitialized=1;
--	end;
--end;
--
------------------------------------------------------------------------------------------------------
--function Shark_x.Client:OnInit()
----	if(not self.bInitialized)then
----		self:OnReset();
----		self.bInitialized=1;
----	end;
--end
--
------------------------------------------------------------------------------------------------------
function Shark_x.Server:OnHit(hit)
end

------------------------------------------------------------------------------------------------------
function Shark_x.Client:OnHit(hit)
	if(not self.dead)then
		local health = self.actor:GetHealth();
		if(health>0) then
			health = health - hit.damage;
			self.actor:SetHealth(health);
			if (health <= 0) then
				self.dead = true;
				self:Die();
			end
		end
	end
end

----------------------------------------------------------------------------------------------------
function Shark_x:Die()
	self:MakeRagDoll();
	self:Bleed();
	self.dead=true;
	self:TriggerEvent(AIEVENT_DISABLE);
	--Script.SetTimer(10000,self.Sink,self);
	self:SetTimer(SHARK_SINK_TIMER,1000);
	-- make another shark spawned ;)
	local wke = self.WaterKillEvent;
	if (wke) then
		wke:SetTimer(WKE_SPAWN_SHARK_TIMER,3000);
	end

end;

----------------------------------------------------------------------------------------------------
function Shark_x:Bleed()
	local fx="misc.blood_fx.water_shark";
	self.FXSlot=self:LoadParticleEffect( -1, fx,{});
end;

function Shark_x:StopBleeding()
	self:FreeSlot(self.FXSlot);
	self.FXSlot= -1;
end;

--function Shark_x:MakeRagDoll()
--	self.actor:SetPhysicalizationProfile("ragdoll");
--
--end

----------------------------------------------------------------------------------------------------
function Shark_x:MakeRagDoll()
	--self:StopAnimation(0,0);
	self:GetVelocity(self.vel);

	local Properties = self.Properties;
	local Physics=self.Physics;

	self.PhysParams.mass = Properties.Mass;
	self.PhysParams.Living = self.DeadBodyParams;

	self:Physicalize( 0,PE_LIVING,self.PhysParams );
	self:Physicalize( 0,PE_ARTICULATED,self.PhysParams );
	self:SetPhysicParams(PHYSICPARAM_SIMULATION, Physics.Simulation);
	self:SetPhysicParams(PHYSICPARAM_BUOYANCY, Physics.Buoyancy);
	self:SetPhysicParams(PHYSICPARAM_SIMULATION, self.Properties );

	local Flags = { flags_mask=pef_fixed_damping+ref_use_simple_solver, flags=0 };
  if Physics.Simulation.bFixedDamping then
		Flags.flags = Physics.Simulation.bFixedDamping*pef_fixed_damping;
	end
	if Physics.Simulation.bUseSimpleSolver then
		Flags.flags = Flags.flags+Physics.Simulation.bUseSimpleSolver*ref_use_simple_solver;
	end
	self:SetPhysicParams(PHYSICPARAM_FLAGS, Flags );

	if (Properties.lying_damping) then
		self.DeadBodyParams.lying_damping = Properties.lying_damping;
	end;
  if (Properties.lying_gravityz) then
		self.DeadBodyParams.lying_gravityz = Properties.lying_gravityz;
	end;
	self:SetPhysicParams(PHYSICPARAM_SIMULATION, self.DeadBodyParams);
	self:SetPhysicParams(PHYSICPARAM_ARTICULATED, self.DeadBodyParams);

	local flagstab = { flags_mask=geom_colltype_player, flags=geom_colltype_player*self.Properties.bCollidesWithPlayers };
	self:SetPhysicParams(PHYSICPARAM_PART_FLAGS, flagstab);
	flagstab.flags_mask = pef_pushable_by_players;
	flagstab.flags = pef_pushable_by_players*self.Properties.bPushableByPlayers;
	self:SetPhysicParams(PHYSICPARAM_FLAGS, flagstab);
	if (self.Properties.bResting == 1) then
		self:AwakePhysics(0);
	else
		self:AwakePhysics(1);
	end

	self:Impulse();
	self:SetTimer(SHARK_IMPULSE_TIMER,100);
end;

function Shark_x:Impulse()
	local speed = LengthVector(self.vel);
	if(speed>0) then
		local velN = g_Vectors.temp;

		FastScaleVector(velN,self.vel,1/speed);
	--	self:AddImpulse( -1, self:GetCenterOfMassPos(), velN, self:GetMass()*speed, 1 );
	end
end

function Shark_x:Sink()
		self:SetPhysicParams(PHYSICPARAM_BUOYANCY,self.SinkBuoyancy);
		self:SetTimer(SHARK_DELETE_TIMER,15000);
end


function Shark_x.Client:OnTimer(timerId)
	if(timerId==SHARK_DELETE_TIMER) then
		-- System.RemoveEntity(self.id);
		self:RemoveActor();
	elseif(timerId==SHARK_SINK_TIMER) then
		self:Sink();
	elseif(timerId==SHARK_IMPULSE_TIMER) then
		self:Impulse();
	end
end
