----------------------------------------------------------------------------------------------------
--  Crytek Source File.
--  Copyright (C), Crytek Studios, 2001-2004.
----------------------------------------------------------------------------------------------------
--  Description: Common functions for actors (players, AIs..)
----------------------------------------------------------------------------------------------------
--  History:
--  20:10:2004 : Created by Filippo De Luca
----------------------------------------------------------------------------------------------------

HOSTAGE_UNTIE = 5;

--shared table for actors
ActorShared =
{
	explosion_death_impulse =
	{ -- explosion impulse
		headshot =
		{
			{
				direction = {x=0,y=0,z=-1},
				strength = 1.2,
				partId = -1,
			},			
			{
				use_direction = true,
				direction = {x=1,y=1,z=1},
				use_strength = true,
				partId = -1,
				strength = 1.2,
			},						
			{
				use_direction = true,
				direction = {x=1,y=1,z=1},
				strength = 1.2,
				partId = 31,
			},
		},
		chestshot =
		{
			{
				direction = {x=0,y=0,z=1},
				use_strength = true,
				partId = -1,
				strength = 1.2,
			},
			{
				use_direction = true,
				direction = {x=1,y=1,z=1},
				use_strength = true,
				partId = -1,
				strength = 1.2,
			},
			{
				use_direction = true,
				direction = {x=1,y=1,z=1},
				use_strength = true,
				partId = 23,
				strength = 1.2,
			},
			{
				use_direction = true,
				direction = {x=1,y=1,z=1},
				use_strength = true,
				partId = 6,
				strength = 1.2,
			},
			{
				use_direction = true,
				direction = {x=-1,y=-1,z=-1},
				use_strength = true,
				partId = 62,
				strength = 0.3,
			},
			{
				use_direction = true,
				direction = {x=-1,y=-1,z=-1},
				use_strength = true,
				partId = 38,
				strength = 0.3,
			},
		},
		rotate = 0.3,
	},
	death_impulses =
	{
		{ -- Light Bullet
			headshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 50,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					strength = 50,
					partId = 31,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 25,
					partId = 2,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 25,
					partId = 3,
				},							
			},
			chestshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 50,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					strength = 5,
				},
			},
			rotate = 0,
		},
		{ -- Shotgun Bullet
			headshot =
			{
				{
					direction = {x=0,y=0,z=-1},
					strength = 500,
					partId = -1,
				},			
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					partId = -1,
					strength = 2,
				},						
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					strength = 150,
					partId = 31,
				},
			},
			chestshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 600,
					use_strength = true,
					partId = -1,
					strength = 10,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					partId = -1,
					strength = 4,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					partId = 23,
					strength = 2,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					partId = 6,
					strength = 2,
				},
				{
					use_direction = true,
					direction = {x=-1,y=-1,z=-1},
					use_strength = true,
					partId = 62,
					strength = 0.3,
				},
				{
					use_direction = true,
					direction = {x=-1,y=-1,z=-1},
					use_strength = true,
					partId = 38,
					strength = 0.3,
				},
			},
			rotate = 0.3,
		},
		{ -- Assault Bullet
			headshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 40,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					strength = 75,
					partId = 31,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 2,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 3,
				},					
			},
			chestshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 30,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					strength = 5,
				},
			},
			rotate = 1,
		},
		{ -- Sniper Bullet
			headshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 40,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					strength = 100,
					partId = 31,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 2,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 3,
				},								
			},
			chestshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 40,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					strength = 2,
				},
			},
			rotate = 1,
		},
		{ -- Hurricane Bullet
			headshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 40,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					strength = 100,
					partId = 31,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 2,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 3,
				},								
			},
			chestshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 100,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					strength = 4,
				},
			},
			rotate = 1,
		},
		{ -- Gauss Bullet
			headshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 150,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					strength = 200,
					partId = 31,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 2,
				},
				{
					direction = {x=0,y=1,z=0},
					strength = 50,
					partId = 3,
				},				
			},
			chestshot =
			{
				{
					direction = {x=0,y=0,z=1},
					strength = 150,
					partId = -1,
				},
				{
					use_direction = true,
					direction = {x=1,y=1,z=1},
					use_strength = true,
					strength = 4.0,
				},
			},
			rotate = 0,
		},
	},
	
  -- why do player sound start from 1 to 10 and AI is 00 to 09 ??
	player =
	{
		pain =
		{
			{"languages/dialog/ai_player/pain_01.wav"},
			{"languages/dialog/ai_player/pain_02.wav"},
			{"languages/dialog/ai_player/pain_03.wav"},
			{"languages/dialog/ai_player/pain_04.wav"},
			{"languages/dialog/ai_player/pain_05.wav"},
			{"languages/dialog/ai_player/pain_06.wav"},
			{"languages/dialog/ai_player/pain_07.wav"},
			{"languages/dialog/ai_player/pain_08.wav"},
			{"languages/dialog/ai_player/pain_09.wav"},
			{"languages/dialog/ai_player/pain_10.wav"},
		},
		
		pain_mp =
		{
			{"languages/dialog/ai_korean_soldier_1/pain_01.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_02.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_03.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_04.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_05.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_06.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_07.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_08.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_09.wav"},
			{"languages/dialog/ai_korean_soldier_1/pain_10.wav"},
		},
		
		death =
		{
			{"languages/dialog/ai_player/death_00.wav"},
			{"languages/dialog/ai_player/death_01.wav"},
			{"languages/dialog/ai_player/death_02.wav"},
			{"languages/dialog/ai_player/death_03.wav"},
			{"languages/dialog/ai_player/death_04.wav"},
			{"languages/dialog/ai_player/death_05.wav"},
			{"languages/dialog/ai_player/death_06.wav"},
			{"languages/dialog/ai_player/death_07.wav"},
			{"languages/dialog/ai_player/death_08.wav"},
			{"languages/dialog/ai_player/death_09.wav"},
		},
		
		death_mp =
		{
			{"languages/dialog/ai_korean_soldier_1/death_00.wav"},		
			{"languages/dialog/ai_korean_soldier_1/death_01.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_02.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_03.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_04.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_05.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_06.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_07.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_08.wav"},
			{"languages/dialog/ai_korean_soldier_1/death_09.wav"},
		},
	},
	 
	alien =
	{
		pain =
		{
			{"sounds/alien:naked_alien:pain"},
		},

		death =
		{
			{"sounds/alien:naked_alien:death"},
		},
		
		tentacles =
		{
			{"sounds/alien:naked_alien:melee_attack"},
		},

		accelerate = {"sounds/alien:naked_alien:accelerate"},
		melee = {"sounds/alien:naked_alien:melee_attack"},
				
		idle = {"sounds/alien:naked_alien:idle"},
	},
	
	alienTest =
	{
		pain =
		{
			{"sounds/alien:naked_alien:pain"},
		},
		
		death =
		{
			{"sounds/alien:naked_alien:death"},
		},
		
		tentacles =
		{
			{"sounds/alien:naked_alien:melee_attack"},
		},
				
		idle = {"sounds/alien:naked_alien:hiss_loop_test"},
	},
	
	trooper =
	{
		pain =
		{
			{"sounds/alien:trooper:pain",{x=0,y=0,z=1.5}},
		},
		
		death =
		{
			{"sounds/alien:trooper:death",{x=0,y=0,z=1.5}},
		},
						
		idle = {"sounds/alien:trooper:idle",{x=0,y=0,z=1.5}},
		idleCloak = {"sounds/alien:trooper:idle_cloaked",{x=0,y=0,z=1.5}},
		
--		tentacles =
--		{
--			{"sounds/alien:trooper:melee_attack"},
--		},
	},
	
	hunter =
	{
		pain =
		{
			{"sounds/alien:hunter:pain"},
		},
		
--		death =
--		{
--			{"sounds/alien:hunter:death_part1"},
--		},
						
		idle = {"sounds/alien:hunter:idle",{x=0,y=0,z=6.5}},
	},
		
	scout =
	{
		pain =
		{
			{"sounds/alien:scout:pain"},
		},
		
		death =
		{
			{"sounds/alien:scout:death"},
		},
		
		idle = {"sounds/alien:scout:idle"},
		
	},
		
	zeroG_sounds =
	{
		thrusters =	{"sounds/player/exosuit/thruster.wav", SOUND_DEFAULT_3D, 100, 3, 30},
		thrusterVolume = 95,
				
		gyro_on = {"sounds/player/exosuit/activating_gyroscope.wav", SOUND_DEFAULT_3D, 250, 3, 30},
		gyro_off = {"sounds/player/exosuit/deactivating_gyroscope.wav", SOUND_DEFAULT_3D, 250, 3, 30},
		
		gboots_on = {"sounds/player/exosuit/activating_magnetic_boots.wav", SOUND_DEFAULT_3D, 250, 3, 30},
		gboots_off = {"sounds/player/exosuit/deactivating_magnetic_boots.wav", SOUND_DEFAULT_3D, 250, 3, 30},
		
		gboots_step =
		{
			{"sounds/player/exosuit/boots_step1.wav", SOUND_DEFAULT_3D, 200, 5, 60},
			{"sounds/player/exosuit/boots_step2.wav", SOUND_DEFAULT_3D, 200, 5, 60},
			{"sounds/player/exosuit/boots_step3.wav", SOUND_DEFAULT_3D, 200, 5, 60},
			{"sounds/player/exosuit/boots_step4.wav", SOUND_DEFAULT_3D, 200, 5, 60},
		}
	},
	
	none = {},
	
	splash_sounds =
	{
		{"Sounds/physics:player_foley:bodyfall_waterdeep", SOUND_DEFAULT_3D, 200, 5, 60},
	},
	
	--animation events
	animation_events =
	{
		{
			"interact_hostageUntie_01",
			{24,HOSTAGE_UNTIE},
		},
	},
}

function GetRandomSound(sounds)
	
	if (type(sounds) == "table") then
		local num = table.getn(sounds);
		
		if (num>=1) then
			if (type(sounds[1]) == "table") then
				return sounds[math.random(num)];
			else
				return sounds;
			end
		end
	end
		
	return nil;
end

function PlayRandomSound(emitter,sounds)
	
	if (not emitter) then 
		return nil; 
	end
	
	local sound = GetRandomSound(sounds);
	
	if (sound) then
		return emitter:PlaySoundEventEx(sound[1], SOUND_DEFAULT_3D, 1, g_Vectors.v000, 0, 0, SOUND_SEMANTIC_AI_READABILITY);
	end
	
	return nil;
end

--various constants
DEAD_TIMER = 1;
DEADANIM_TIMER = 2;
THROWOBJECT_TIMER = 3;
EQUIP_TIMER = 4;
ACTOREFFECTS_TIMER = 5;
BLEED_TIMER = 9;
PAIN_TIMER = 10;
NANOSUIT_MODE_TIMER = 11;
NANOSUIT_CLOAK_TIMER = 12;
NANOSUIT_UNCLOAK_TIMER = 13;
DEATH_REIMPULSE_TIMER = 14;
COLLISION_TIMER = 15;
UNRAGDOLL_TIMER = 16;
HOSTAGE_COWER_TIMER = 17;
SWITCH_WEAPON_TIMER = 18;
BLOOD_POOL_TIMER = 19;

--FIXME:these have to be renamed as ACTOREVENT_...
ALIENEVENT_PLAYANIMATION = 1;
ALIENEVENT_ISVULNERABLE = 2;
ALIENEVENT_FIREPRIMARY = 3;
ALIENEVENT_STOPANIMATION = 4;
ALIENEVENT_QUEUEGRAPHSTATE = 5;
ALIENEVENT_CALLFUNCTION = 6;
ALIENEVENT_SETGRAPHINPUT = 7;


	
----------------------------------------------------------------------------------------------------
--BasicActor
BasicActor =
{	
	AnimationGraph = "PlayerFullBody.xml",
	UpperBodyGraph = "PlayerUpperBody.xml",

	Properties = 
	{
		soclasses_SmartObjectClass = "Actor",
		ragdollPersistence = 0,
		physicMassMult = 1.0,
		
		Damage =
		{
			bLogDamages = 0,
			health = 100,
			--playerMult = 1.0,
			--AIMult = 1.0,
		},
		
		equip_EquipmentPack="",
	},
		
	lastHit =
	{
		dir = {x=0,y=0,z=0},
		pos = {x=0,y=0,z=0},
		velocity = {x=0,y=0,z=0},
		partId = -1,
		damage = 0,
		bulletType = -1,
	},
	
	tempSetStats =
	{
	},
			
	Server = {},
	Client = {},
	
	grabParams =
	{
		collisionFlags = 0, --geom_colltype_player,		
						
		holdPos = {x=0.0,y=0.4,z=1.25}, -- position where grab is holded
						
		grabDelay = 0,--if IK is used, its the time delay where the limb tries to reach the object.
		followSpeed = 5.5,
		
		limbs = 
		{
			"rightArm",
			"leftArm",
			--"fpRightArm",
			--"fpLeftArm",
		},
		
		useIKRotation = 0,
	},
	
--	Bip01 R Clavicle
--	Bip01 R UpperArm
--	Bip01 R Forearm
--	Bip01 R Hand

	IKLimbs = 
	{
		{0,"rightArm","Bip01 R UpperArm","Bip01 R Forearm","Bip01 R Hand", IKLIMB_RIGHTHAND},
		{0,"leftArm","Bip01 L UpperArm","Bip01 L Forearm","Bip01 L Hand", IKLIMB_LEFTHAND},
				
		--{2,"fpRightArm","rootR","forearm_R","hand_R"},
		--{2,"fpLeftArm","rootL","forearm_L","hand_L"},
		
		--{2,"fpRightArm","rootR","","hand_R"},
		--{2,"fpLeftArm","rootL","","hand_L"},
	},
		
	bloodFlowEffectWater = "misc.blood_fx.water",
	bloodFlowEffect = "misc.blood_fx.ground",
	bloodSplatWall={	
		"Materials/decals/blood_splat1",
		"Materials/decals/blood_splat2",
		"Materials/decals/blood_splat5",
		"Materials/decals/blood_splat7",
		"Materials/decals/blood_splat11",
	},
	bloodSplatGround={	
		"materials/decals/blood_pool",
	},
	bloodSplatGroundDir = {x=0, y=0, z=-1},

	waterStats =
	{
		lastSplash = 0,
	},
	
	actorStats =
	{
		
	},
}


BasicActorParams =
{	
	physicsParams =
	{
		flags = 0,
		mass = 80,
		stiffness_scale = 73,
					
		Living = 
		{
			gravity = 9.81,--15,--REMINDER: if there is a ZeroG sphere in the map, gravity is set to 9.81.
						 --It will be fixed, but for now just do all the tweaks without any zeroG sphere in the map.
			mass = 80,
			air_resistance = 0.5, --used in zeroG
			
			k_air_control = 0.9,
			
			max_vel_ground = 16,
			
			min_slide_angle = 45.0,
			max_climb_angle = 50.0,
			min_fall_angle = 50.0,
			
			timeImpulseRecover = 1.0,
			
			colliderMat = "mat_player_collider",
		},
	},
	
	gameParams =
	{
		stance =
		{
			{
				stanceId = STANCE_STAND,
				normalSpeed = 1.0,
				maxSpeed = 5.0,
				heightCollider = 1.2,
				heightPivot = 0.0,
				size = {x=0.4,y=0.4,z=0.2},
				modelOffset = {x=0,y=-0.0,z=0},
				viewOffset = {x=0,y=0.10,z=1.625},
				weaponOffset = {x=0.2,y=0.0,z=1.35},
				leanLeftViewOffset = {x=-0.5,y=0.10,z=1.525},
				leanRightViewOffset = {x=0.5,y=0.10,z=1.525},
				leanLeftWeaponOffset = {x=-0.45,y=0.0,z=1.30},
				leanRightWeaponOffset = {x=0.65,y=0.0,z=1.30},
				name = "combat",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_STEALTH,
				normalSpeed = 0.6,
				maxSpeed = 3.0,
				heightCollider = 1.0,
				heightPivot = 0.0,
				size = {x=0.4,y=0.4,z=0.1},
				modelOffset = {x=0.0,y=-0.0,z=0},
				viewOffset = {x=0,y=0.3,z=1.35},
				weaponOffset = {x=0.2,y=0.0,z=1.1},
				name = "stealth",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_CROUCH,
				normalSpeed = 0.5,
				maxSpeed = 3.0,
				heightCollider = 0.8,
				heightPivot = 0.0,
				size = {x=0.4,y=0.4,z=0.1},
				modelOffset = {x=0.0,y=0.0,z=0},
				viewOffset = {x=0,y=0.0,z=1.1},
				weaponOffset = {x=0.2,y=0.0,z=0.85},
				leanLeftViewOffset = {x=-0.55,y=0.0,z=0.95},
				leanRightViewOffset = {x=0.55,y=0.0,z=0.95},
				leanLeftWeaponOffset = {x=-0.5,y=0.0,z=0.65},
				leanRightWeaponOffset = {x=0.5,y=0.0,z=0.65},
				name = "crouch",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_PRONE,
				normalSpeed = 0.5,
				maxSpeed = 1.0,
				heightCollider = 0.5,
				heightPivot = 0.0,
				size = {x=0.4,y=0.4,z=0.01},
				modelOffset = {x=0,y=0.0,z=0},
				viewOffset = {x=0,y=0.5,z=0.35},
				weaponOffset = {x=0.1,y=0.0,z=0.25},
				name = "prone",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_SWIM,
				normalSpeed = 1.0, -- this is not even used?
				maxSpeed = 2.5, -- this is ignored, overridden by pl_swim* cvars.
				heightCollider = 0.9,
				heightPivot = 0.5,
				size = {x=0.4,y=0.4,z=0.1},
				modelOffset = {x=0,y=0,z=0.0},
				viewOffset = {x=0,y=0.1,z=0.5},
				weaponOffset = {x=0.2,y=0.0,z=0.3},
				name = "swim",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_ZEROG,
				normalSpeed = 1.75,
				maxSpeed = 3.5,
				heightCollider = 1.2,
				heightPivot = 1.0,
				size = {x=0.4,y=0.4,z=0.6},
				modelOffset = {x=0,y=0,z=-1},
				viewOffset = {x=0,y=0.15,z=0.625},
				weaponOffset = {x=0.2,y=0.0,z=1.3},
				name = "combat",
				useCapsule = 1,
			},
			--AI states
			{
				stanceId = STANCE_RELAXED,
				normalSpeed = 1.0,
				maxSpeed = 1.9,
				heightCollider = 1.2,
				heightPivot = 0.0,
				size = {x=0.4,y=0.4,z=0.2},
				modelOffset = {x=0,y=0,z=0},
				viewOffset = {x=0,y=0.10,z=1.625},
				weaponOffset = {x=0.2,y=0.0,z=1.3},
				name = "relaxed",
				useCapsule = 1,
			},
		},
					
		sprintMultiplier = 1.5,--speed is multiplied by this ammount if sprint key is pressed -- 1.2 for a more counter-striky feel
		strafeMultiplier = 1.0,--speed is multiplied by this ammount when strafing
		backwardMultiplier = 0.7,--speed is multiplied by this ammount when going backward
		grabMultiplier = 0.5,--speed is multiplied by this ammount when the player is carry the maximun ammount carriable
				
		inertia = 10.0,--7.0,--the more, the faster the speed change: 1 is very slow, 10 is very fast already 
		inertiaAccel = 11.0,--same as inertia, but used when the player accel
			
		jumpHeight = 1.0,--meters
		
		slopeSlowdown = 3.0,
		
		leanShift = 0.35,--how much the view shift on the side when leaning
		leanAngle = 15,--how much the view rotate when leaning
		
		--ZeroG stuff:
		thrusterImpulse = 14.0,--thruster power, for the moment very related to air_resistance.
		thrusterStabilizeImpulse = 0.01,--used from the jetpack to make the player stop slowly.
		gravityBootsMultipler = 0.8,--speed is multiplied by this ammount when gravity boots are on
		afterburnerMultiplier = 2.0,--how much the afterburner
		
		--grabbing
		maxGrabMass = 70,
		maxGrabVolume = 2.0,--its the square volume of an 1x1x2 meters object
	},
}

function CreateActor(child)
	mergef(child,BasicActorParams,1);
	mergef(child,BasicActor,1);
end

function BasicActor:RemoveActor()
	--in the editor just hide the entity when the game need it to be removed.
	if (System.IsEditor()) then
		self:Hide(1);
		self:ShutDown();
	else
		self.actor:SetHealth(0);
		self:DestroyPhysics();
		System.RemoveEntity(self.id);
	end
end


--TODO/FIXME:there should be BasicHuman and BasicAlien derived from BasicActor that inherit the Reset function, not this..
function BasicActor:ResetCommon()
	--self:Hide(0);
	
	--revive it
	self.actor:Revive();

	--set health
	local health = self.Properties.Damage.health;
	if (g_gameRules and not g_gameRules:IsMultiplayer() and self.actor:IsPlayer()) then
		health = System.GetCVar("g_playerHealthValue");
	end
	self.actor:SetMaxHealth(health);
	self.lastHealth = self.actor:GetHealth();
	
	--sounds
	self:StopSounds();
	BasicActor.InitSoundTables(self);
	
	--reset events
	self:StopEvent("all");
	
	-- drop grab, if present
	self:DropObject();
	
	--
	self:RemoveDecals();
	self:EnableDecals(0, true);
	
	--
	self.lastDeathImpulse = 0;
	self.bodyUnseen = 0;
	self:KillTimer(DEAD_TIMER);
	self:KillTimer(DEADANIM_TIMER);
	
	self:KillTimer(PAIN_TIMER);
	self:KillTimer(BLOOD_POOL_TIMER);
	self.painSoundTriggered = nil;
	
	--to save some performace, the effects timer for AI will be different.
	if (self.actor:IsPlayer()) then
		self:SetTimer(ACTOREFFECTS_TIMER, 100);
	else
		self:SetTimer(ACTOREFFECTS_TIMER, 500);
	end
			
	if (self.lastSpawnPoint) then
		self.lastSpawnPoint = 0;
	end
	self.AI = {};

	if (not System.IsEditor()) then	
		if (self.inventory and self.ammoCapacity) then
			for ammo,capacity in pairs(self.ammoCapacity) do
				self.inventory:SetAmmoCapacity(ammo, capacity);
			end
		end
	end
	
	
end

--------------------------------------------------------------------------
function BasicActor:GetCollisionDamageThreshold()      
  return self.collisionDamageThreshold or 0;
end

--------------------------------------------------------------------------
function BasicActor:GetSelfCollisionMult(collider, hit) 
	if (not collider) then  
		-- static geometry -> use self collision damping
		return self.selfCollisionDamageMult or 1;  
	end

	if(collider and collider.vehicle) then
		if(g_gameRules:IsMultiplayer()) then
			return self.vehicleCollisionDamageMultMP or 1;
		else
			return self.vehicleCollisionDamageMult or 1;
		end
	end

	return self.entityCollisionDamageMult or 1;
end

--------------------------------------------------------------------------
function BasicActor:GetForeignCollisionMult(entity, hit)
	if (self.foreignCollisionDamageMult) then
		return self.foreignCollisionDamageMult;
	end
  
	return 1;
end

--------------------------------------------------------------------------
function BasicActor:GetColliderEnergyScale(collider)
	if (not collider.actor) then
		return self.colliderEnergyScale or 1; 
	elseif ((collider.actor:GetPhysicalizationProfile() == "sleep") or (collider.actor:GetPhysicalizationProfile() == "ragdoll")) then
		return self.colliderRagdollScale or 1;
	end

	return 1;
end

function BasicActor:Reset()

	BasicActor.ResetCommon(self);
		
	--misc resetting
	self.actor:SetMovementTarget(g_Vectors.v000,g_Vectors.v000,g_Vectors.v000,1);
	self.lastVehicleId = nil;
	self.AI.theVehicle = nil;
		
	self:ResetBleeding();
	
	--E3 hacks
	BasicActor.ActorLink(self);
end

function BasicActor:ResetLoad()
	--sound tables
	BasicActor.InitSoundTables(self);
end

function BasicActor:InitAnimationEvents()
	
	self.animation_events = ActorShared.animation_events;
	
	--TODO:check all possible nils and such
	if (self.animation_events) then
		
		local chrSlot = 0;
		
		for i,event in pairs(self.animation_events) do
			
			--self:SetAnimationEvent(chrSlot,event[1]);
			local cells = table.getn(event);
			
			for i=2, cells do
				
				local cell = event[i];
				self:SetAnimationKeyEvent(chrSlot,event[1],cell[1],cell[2]);				
			end
		end
	end
end

function BasicActor:InitIKLimbs()
	if (self.IKLimbs) then		
		for i,limb in pairs(self.IKLimbs) do
			self.actor:CreateIKLimb(limb[1],limb[2],limb[3],limb[4],limb[5],limb[6] or 0);
		end
	end
end

function BasicActor:SetAnimKeyEvent(animation,frame,func)
	if (animation and animation ~="") then
		if (not self.onAnimationKey) then
			self.onAnimationKey = {};
		end
		self.onAnimationKey[animation..frame] = func;
		self:SetAnimationKeyEvent(0, animation,frame,animation..frame);
	end
end

function BasicActor:InitSoundTables()
	
	local voiceType = self.Properties.voiceType;
	if (not voiceType or voiceType == "") then
		voiceType = self.voiceType;
	end
	
	if (voiceType and voiceType ~= "" and ActorShared[voiceType]) then
		
		self.voiceTable = ActorShared[voiceType];
	 	local precache = GetRandomSound(self.voiceTable.pain);
		if (precache~=nil) then 
	 		Sound.Precache(precache[1], SOUND_PRECACHE_LOAD_SOUND);
	 	end;
--	else
--		self.voiceTable = {};
--		AI.Warning("Warning, "..self:GetName().." has no voice table!");
	end
	
	self.zeroGTable = ActorShared.zeroG_sounds;
	
end


function BasicActor:ShutDown()  
	self:ResetAttachment(0, "right_item_attachment");
	self:ResetAttachment(0, "left_item_attachment");	
	self:ResetAttachment(0, "laser_attachment");
	--self:DestroyAttachment(0,"mouth");
end


function BasicActor.Client:OnUpdate(frameTime)
  
  -- update screen frost. 
  -- in MP, we only have frost after being fully frozen, in SP always if frozenAmount > 0 (by Design)
  local frozenAmount = self.actor:GetFrozenAmount();
  local doFrost = self.actorStats.isFrozen or (frozenAmount>0.05 and not g_gameRules:IsMultiplayer());
  local isClient = self.actor:IsLocalClient();
  
  if (doFrost and frozenAmount < 1) then
  	if (g_gameRules:IsMultiplayer()) then
  		doFrost=false;
  	end
  end

	if (doFrost) then
		if (self.actorStats.isFrozen) then
		  if (not self._frost_params) then self._frost_params={}; end;
		  self._frost_params.speedMultiplier = 1-frozenAmount;
		  self.actor:SetParams(self._frost_params);
		end
		
		if (isClient) then	  	
			System.SetScreenFx("ScreenFrost_CenterAmount", 1);		  			
			System.SetScreenFx("ScreenFrost_Amount", math.min(1, 1.5*frozenAmount));
		end
	elseif (isClient and self.prevFrozenAmount > 0.1 and frozenAmount <= 0.1) then
		System.SetScreenFx("ScreenFrost_CenterAmount", 0);		  			
		System.SetScreenFx("ScreenFrost_Amount", 0);
	end
	
	self.prevFrozenAmount = frozenAmount;
end


function BasicActor.Server:OnUpdate(frameTime)
	if (not self:IsDead()) then
		self:UpdateEvents(frameTime);
	end
end


function GetRangeRatio(num,min,max)
	local delta = max - min;
	return (math.min(1.0,math.max(0,(num-min)/delta)));
end

function BasicActor:UpdateSounds(frameTime)
	local aStats = self.actorStats;
	
	--stance sounds
	local newStance = aStats.stance or 0;
	local oldStance = aStats.oldStance or 0;
	
	if (newStance ~= oldStance) then
		local stanceSound = "";
		--
		if (newStance == STANCE_CROUCH or newStance == STANCE_STEALTH) then
			stanceSound = "sounds/physics:player_foley:crouch_on";
		elseif (newStance == STANCE_PRONE) then
			stanceSound = "sounds/physics:player_foley:prone_on";
		else
			--standup
			if (oldStance == STANCE_PRONE) then
				stanceSound = "sounds/physics:player_foley:prone_off";
			else
				stanceSound = "sounds/physics:player_foley:crouch_off";
			end
		end
		
		local sound = self:PlaySoundEvent(stanceSound, g_Vectors.v000, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_PLAYER_FOLEY);
		
		--Log("stance changed, last:"..oldStance..", current:"..newStance);
	end
	
	aStats.oldStance = newStance;
end

function BasicActor:WallBloodSplat(hit)
	local blood = tonumber(System.GetCVar("g_blood"));
	if (blood == 0) then
		return;
	end
	if (hit.material) then
		local dist = 2.5;
		local dir = vecScale(hit.dir, dist);
		local	hits = Physics.RayWorldIntersection(hit.pos,dir,1,ent_all,hit.targetId,nil,g_HitTable);
		
		local splat = g_HitTable[1];
		if (hits > 0 and splat and ((splat.dist or 0)>0.25)) then
			local n = table.getn(self.bloodSplatWall);
			local i = math.random(1,n);
			local s = 0.25+(splat.dist/dist)*0.35;
			
			Particle.CreateMatDecal(splat.pos, splat.normal, s, 300, self.bloodSplatWall[i], math.random()*360, vecNormalize(hit.dir), splat.entity and splat.entity.id, splat.renderNode);
		end
	end
end

function BasicActor:DoBloodPool()
	local blood = tonumber(System.GetCVar("g_blood"));
	if (blood == 0) then
		return;
	end

	-- if we are still moving, let's not do a bloodpool, yet
	self:GetVelocity(g_Vectors.temp_v1);

	if (LengthSqVector(g_Vectors.temp_v1) > 0.2) then
		self:SetTimer(BLOOD_POOL_TIMER,1000);
		return;
	end
	self:KillTimer(BLOOD_POOL_TIMER);
	
	local dist = 2.5;
	local pos = self:GetBonePos("Bip01 Spine2", g_Vectors.temp_v1);	
	--local pos = self:GetWorldPos(g_Vectors.temp_v1);	
	if(pos == nil) then
		Log("Bip01 Spine2 not found in model "..self.currModel);
		return;
	end
	pos.z = pos.z + 1;
	local dir = vecScale(self.bloodSplatGroundDir, dist);
	local	hits = Physics.RayWorldIntersection(pos,dir,1,ent_terrain+ent_static, nil, nil, g_HitTable);
	
	local splat = g_HitTable[1];
	if (hits > 0 and splat) then
		local n = table.getn(self.bloodSplatGround);
		local i = math.random(1,n);	
		local s = 0.4;--random(0.4, 0.45);
		--local s = 0.5+(splat.dist/dist)*0.5;
		
		Particle.CreateMatDecal(splat.pos, splat.normal, s, 300, self.bloodSplatGround[i], math.random()*360, vecNormalize(dir), splat.entity and splat.entity.id, splat.renderNode, 6, true);
		--Particle.CreateDecal(splat.pos, splat.normal, 0.9, 300, "textures/sprites/blood/blood_pool.tif", math.random()*360, vecNormalize(hit.dir), splat.entity and splat.entity.id, splat.partid, 4);
	end
end

function BasicActor:StartBleeding()
	local blood = tonumber(System.GetCVar("g_blood"));
	if (blood == 0) then
		return;
	end
	self:DestroyAttachment(0, "wound");
	self:CreateBoneAttachment(0, "Bip01", "wound");
	
	local effect=self.bloodFlowEffect;
	local pos = self:GetBonePos("Bip01", g_Vectors.temp_v1);
	local level,normal,flow=CryAction.GetWaterInfo(pos);
	if (level and level>=pos.z) then
		effect=self.bloodFlowEffectWater;
	end
	
	self.bleeding = true;
	self:SetAttachmentEffect(0, "wound", effect, g_Vectors.v000, g_Vectors.v010, 1, 0);
end


function BasicActor:StopBleeding()
	if (self.bleeding == true) then
		self:DestroyAttachment(0, "wound");
	end
	self.bleeding=false;
end


function BasicActor:IsBleeding()
	return self.bleeding;
end


function BasicActor:Deplete(deplete)
	self.deplete = deplete;
end


function BasicActor:IsDepleted()
	return self.depleted;
end


function BasicActor:ResetBleeding()
	self.depleted = false;
	self.bleeding = false;
	self:StopBleeding();

	self:KillTimer(BLEED_TIMER);
end


function BasicActor:PushEvent(eventStr)
	
	local events = self.scripted_events;
	
	if (not events) then
		return 0;
	end
	
	local event = events[eventStr];
	
	if (event) then
		
		if (not self.events) then
			self.events = {};
		end
		
		local asize = table.getn(self.events);
		local newIdx = asize+1;
		
		for i=1,asize do
			if (not self.events[i].event) then	
				newIdx = i;
				break;
			end
		end
			
		--
		if (newIdx == asize+1) then
			self.events[newIdx] = {};
		end
		
		self.events[newIdx].eventName = eventStr;
		self.events[newIdx].event = event;
		self.events[newIdx].time = 0;
		self.events[newIdx].currentEventId = 1;
		
		AI.LogEvent("event "..eventStr.." pushed at index "..newIdx.."!!");
				
		return 1;
	end
end

function BasicActor:IsEventActive(eventStr)
	
	if (self.events) then
				
		local asize = table.getn(self.events);
		
		for i=1,asize do
			if (self.events[i].event) then	
				if (self.events[i].eventName == eventStr) then
					return 1;
				end
			end
		end
	end
	
	return nil;
end

function BasicActor:UpdateEvents(frameTime)
	
	if (self.events) then
		
		local event = nil;
		local asize = table.getn(self.events);
		
		for i=1,asize do
			if (self.events[i].event) then	
					
				event = self.events[i];
				if (event.time >= 0) then
					event.time = event.time + frameTime;
				end
			
				for j,step in pairs(event.event) do
			
					--if time is negative we want to stop this event
					if ((event.time<0 and j>=event.currentEventId) or (step[2]<=event.time and j == event.currentEventId)) then
					
--						if (event.time < 0) then
--							AI.LogEvent("stopping event "..event.eventName.."("..event.currentEventId..")!");
--						end
							
						self:PlayEvent(step[1]);
						event.currentEventId = event.currentEventId + 1;
						
						if (event.currentEventId > table.getn(event.event)) then
							self.events[i].event = nil;
							AI.LogEvent("event "..event.eventName.." stopped!");
						end
					end	
				end
			end
		end
	end
end

--if eventStr is "all" stop all events
function BasicActor:StopEvent(eventStr)
		
	if (self.events) then
				
		local asize = table.getn(self.events);
		
		for i=1,asize do
			if (self.events[i].event) then	
				if (eventStr == "all" or self.events[i].eventName == eventStr) then
					self.events[i].time = -1;
					AI.LogEvent("REQUEST TO STOP event "..self.events[i].eventName);
				end
			end
		end
		
		--make sure to stop events
		self:UpdateEvents(1.0);
	end
end

function BasicActor:PlayEvent(eventStr)
	
	local events = self.scripted_events;
	
	if (not events) then
		return 0;
	end
	
	local event = events[eventStr];
	
	if (event) then
		
		if (event.event == ALIENEVENT_CALLFUNCTION) then
		  if (event.func) then
			  event.func(self,event.value);
			else
			  Log("<%s> BasicActor:PlayEvent: func not found! (event %s)", self:GetName(), eventStr);
			end		
		elseif (event.event == ALIENEVENT_QUEUEGRAPHSTATE) then
			self.actor:QueueAnimationState(event.value);
		elseif (event.event == ALIENEVENT_SETGRAPHINPUT) then
			self.actor:SetAnimationInput( event.value, event.value2 );
		elseif (event.event == ALIENEVENT_PLAYANIMATION) then
			self:StartAnimation(0, event.value, event.value4, event.value2, event.value3, event.value5, true);
			--Log("ALIENEVENT_PLAYANIMATION");
		elseif (event.event == ALIENEVENT_STOPANIMATION) then
			self:StopAnimation(0, event.value);
			--Log("ALIENEVENT_STOPANIMATION");
		elseif (event.event == ALIENEVENT_ISVULNERABLE) then
			
			if (event.value == 1) then
				self.isVulnerable = 1;
			else
				self.isVulnerable = nil;
			end
			--Log("ALIENEVENT_ISVULNERABLE");
		elseif (event.event == ALIENEVENT_FIREPRIMARY) then
			self.actor:SimulateOnAction("attack1",event.value,1);
			--Log("ALIENEVENT_FIREPRIMARY");
		end
	end
end

function BasicActor:WalkingOnWater()
end

function BasicActor:HealthChanged()	

	local health = self.actor:GetHealth();
	local damage = self.lastHealth - health;

	if (self.Properties.Damage.bLogDamages ~= 0) then
		Log(self:GetName().." health:"..health.." (last damage:"..damage..")");
	end
	
	self.lastHealth = self.actor:GetHealth();
end

function BasicActor.Server:OnDeadHit(hit)
	--Log("BasicActor.Server:OnDeadHit()");
	local frameID = System.GetFrameID();
	if ((frameID - self.lastDeathImpulse) > 10) then
		--marcok: talk to me before touching this
		local dir = g_Vectors.temp_v2;
		CopyVector(dir, hit.dir);
		dir.z = dir.z + 1;
		--dir.x = random(-0.1, 0.1); dir.y = random(-0.1, 0.1); dir.z = 1;

		local angAxis = g_Vectors.temp_v3;
		angAxis.x = math.random() * 2 - 1;
		angAxis.y = math.random() * 2 - 1;
		angAxis.z = math.random() * 2 - 1;
		
		local imp1 = math.random() * 20 + 10;
		local imp2 = math.random() * 20 + 10;
		--System.Log("DeadHit linear: "..string.format(" %.03f,%.03f,%.03f",dir.x,dir.y,dir.z).." -> "..tostring(imp1));
		--System.Log("DeadHit angular: "..string.format(" %.03f,%.03f,%.03f",angAxis.x,angAxis.y,angAxis.z).." -> "..tostring(imp2));
		
		self:AddImpulse( hit.partId, hit.pos, dir, imp1, 1, angAxis, imp2, 35);
		self.lastDeathImpulse = frameID;
	end 
end


function BasicActor.Server:OnHit(hit)	
	if (self.actor:GetSpectatorMode()~=0) then
		return;
	end

	if (hit.damage>=1) then
		if (g_gameRules and hit.target and hit.target.actor and hit.target.actor:IsPlayer()) then
			if(not hit.shooterId == nil or (hit.damage > 3.0)) then
				g_gameRules.game:SendDamageIndicator(hit.targetId, hit.shooterId or NULL_ENTITY, hit.weaponId or NULL_ENTITY);	
			end
		end
		if (hit.shooter and hit.shooter.actor and hit.shooterId~=hit.targetId and hit.shooter.actor:IsPlayer()) then
			if (g_gameRules) then
				g_gameRules.game:SendHitIndicator(hit.shooterId, hit.explosion or false);
			end
		end
	end
	
  --Log("BasicActor.Server:OnHit (%s)", self:GetName());
	-- store some information for musiclogic	
	if (not self.MusicInfo) then self.MusicInfo={}; end
	self.MusicInfo.nonbullet = true;
	
	if (self:IsOnVehicle() and hit.type~="heal") then
		local vehicle = System.GetEntity(self.actor:GetLinkedVehicleId());				
		local newDamage = vehicle.vehicle:ProcessPassengerDamage(self.id, self.actor:GetHealth(), hit.damage, hit.type or "", hit.explosion or false);
		if (newDamage <= 0.0) then		  
			return;
		end		
	end
	
	local isPlayer = self.actor:IsPlayer();
	
	if (hit.damage>0) then
		self.actor:NanoSuitHit(hit.damage);
	end
	
--	if (self.actor:GetPhysicalizationProfile() == "sleep") then
--		self.actor:StandUp();
--	end
	
	if (hit.frost and hit.frost>0) then
	  self.actor:AddFrost(hit.frost);
	end
	
--	Log("OnHit >>>>>>>>> "..self:GetName().."   damage: "..hit.damage);

	local died = g_gameRules:ProcessActorDamage(hit);
		
  if (died and not isPlayer and (hit.type == "collision" or hit.explosion == true)) then
    self:LastHitInfo(self.lastHit, hit);
	end
	
	if(hit.damage==0 or hit.type=="heal") then return end
		
	--some AI related
	if (not isPlayer) then	
		local theShooter=hit.shooter;
		-- upade hide-in-vehicle
		if (theShooter and theShooter.IsOnVehicle) then
			local shootersVehicleId = theShooter:IsOnVehicle();
			if (shootersVehicleId) then
				local shootersVehicle = System.GetEntity(shootersVehicleId);
				if(shootersVehicle and shootersVehicle.ChangeSpecies and (shootersVehicle.AI==nil or shootersVehicle.AI.hostileSet~=1)) then
					shootersVehicle:ChangeSpecies(theShooter, 2);
				end
			end	
		end
	
		if (hit.type and hit.type ~= "collision" and hit.type ~= "fall" and hit.type ~= "event") then
			if (theShooter) then
				CopyVector(g_SignalData.point, theShooter:GetWorldPos());
				g_SignalData.id = hit.shooterId;
			else
				g_SignalData.id = NULL_ENTITY;
				CopyVector(g_SignalData.point, g_Vectors.v000); 
			end	

			g_SignalData.fValue = hit.damage;
			
			if (theShooter and AI.Hostile(self.id,hit.shooterId)) then
				AI.Signal(SIGNALFILTER_SENDER,0,"OnEnemyDamage",self.id,g_SignalData);
				AI.UpTargetPriority(self.id, hit.shooterId, 0.2);	-- make the target more important
				-- check for greeting player in case of "nice shot"
				if(died and theShooter == g_localActor) then 
					local ratio = self.lastHealth / self.Properties.Damage.health;
					if( ratio> 0.9 and hit.material_type and hit.material_type=="head" and hit.type and hit.type=="bullet") then 
						AI.Signal(SIGNALFILTER_GROUPONLY,0,"OnPlayerNiceShot",g_localActor.id);
					end
				end
--			elseif(hit.shooter and hit.shooter==g_localActor and self.Properties.species==hit.shooter.Properties.species) then
--				AI.Signal(SIGNALFILTER_SENDER,0,"OnFriendlyDamageByPlayer",self.id,g_SignalData);
			elseif (theShooter ~= nil and theShooter~=self) then
				if(hit.weapon and hit.weapon.vehicle) then 
					AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
				else
					AI.Signal(SIGNALFILTER_SENDER,0,"OnFriendlyDamage",self.id,g_SignalData);
				end
			else
				AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
			end
		end
		
		if (self.RushTactic) then
			self:RushTactic(5);
		end
		
	--elseif (not died) then
--		Log("Player Damage: damage="..hit.damage.." t=".._time);
		-- design request : decouple energy from health
		--self.actor:CreateCodeEvent({event = "addSuitEnergy",amount=-hit.damage*0.2});
	end

	--AI.DebugReportHitDamage(self.id, hit.shooter.id, hit.damage, hit.material_type);
	
-- kirill > this is for debugging AI accurracy, do not remove	
--	self.actor:CreateCodeEvent({event = "aiHitDebug",shooterId=hit.shooterId});	
	
--	if (died) then
--		self:SetTimer(UNRAGDOLL_TIMER, 3500);
--	end
	
	self:HealthChanged();
	return died;
end

function BasicActor:ApplyDeathImpulse()
	
	local lastVelocity = self.lastHit.velocity;
	local lastSpeed = LengthVector(lastVelocity);
		
	local dir = g_Vectors.temp_v2;
		
	if (self.lastHit.dir) then
		CopyVector(dir, self.lastHit.dir);
	else
		dir.x = 0;
		dir.y = 0;
		dir.z = 0;
	end
		
	dir.z = dir.z + (math.random()*6 + 1)*0.1;
		
	--NormalizeVector(lastVelocity);
	--FastSumVectors(dir, dir, lastVelocity);
							
	--local impulse = 2.0*lastSpeed + (100*math.random(1.0,2.0));
	if (self.lastHit.damage < 0.01) then
		return;
	end
	
	local damageDone = self.lastHit.damage;
	local impulse = math.min(650,math.max(10,self.lastHit.damage) * (math.random()*3+6));
			
	local playerPos = {};
	self:GetWorldPos(playerPos);
	playerPos.z = playerPos.z + 1;
	
	local hitpos = g_Vectors.temp_v3;
	local partId = self.lastHit.partId;
	
	if (not partId) then
		partId = -1;
	end
			
	if (self.lastHit.pos) then
		CopyVector(hitpos,self.lastHit.pos);
	else
		CopyVector(hitpos,playerPos);
	end
		
		
	if (self.lastMelee) then
	
		--local dir = g_Vectors.temp_v4;
		--g_localActor.actor:GetHeadDir(dir) --not working in MP - talk to Jan N
		
		if (dir.z < 0) then
			dir.z = 0.1;
			NormalizeVector(dir);
		end
		
		if(self.lastMeleeImpulse > 300) then
			self:AddImpulse( -1, hitpos, dir, self.lastMeleeImpulse*1.2,1 );
		else
			self:AddImpulse( partId, hitpos, dir, self.lastMeleeImpulse*1.5,1 );
		end
		self.lastMelee = nil;
	else
		local fwdVec = {x=0,y=0,z=0};
		local rightVec = {x=0,y=0,z=0};
		
		self:GetDirectionVector(0,rightVec);
		self:GetDirectionVector(1,fwdVec);
		
		local rotAxis = g_Vectors.temp_v4;
		--CopyVector(rotAxis,g_Vectors.v001);
		self:GetDirectionVector(2,rotAxis);
		rotAxis.x = rotAxis.x - fwdVec.x * 0.35; 
		rotAxis.y = rotAxis.y - fwdVec.y * 0.35;
		rotAxis.z = rotAxis.z - fwdVec.z * 0.35;
		NormalizeVector(rotAxis);
				
		local delta = {x=0,y=0,z=0};
		SubVectors(delta,hitpos,playerPos);
		delta.z = 0;
						
		local dotRight = math.min(1.0,math.max(-1.0,dotproduct3d(rightVec,delta)));
		local dotFront = math.min(1.0,math.max(-1.0,dotproduct3d(fwdVec,delta)));
		
		local angImpulse = -1;
		if(dotRight>0)then
			--angImpulse = -1;
		end
		if(dotFront<0)then
			angImpulse = angImpulse * -1;
		end
		
		--Log("angImpulse = "..angImpulse);
		
		angImpulse = angImpulse * (dotRight * math.pi) * (impulse/650) * 80;

		--cap the angular impulse if necessary
		local angLen = math.abs(angImpulse/self.physicsParams.mass);
		if (angLen > (math.pi*0.5)) then
			angImpulse = (angImpulse / (angLen*self.physicsParams.mass)) * math.pi * 0.5 * self.physicsParams.mass;
		end

		if(self.lastHit.bulletType>0 or self.lastHit.explosion)then
			local deathImpulse = ActorShared.death_impulses[self.lastHit.bulletType];
			if (self.lastHit.explosion) then
				deathImpulse = ActorShared.explosion_death_impulse;
			end
			if(deathImpulse ~= nil)then
				local lookUp;
				if(partId == 31)then
					lookUp = deathImpulse.headshot;
				else
					if(deathImpulse.rotate ~= nil and deathImpulse.rotate>0)then
						--Log("rotate -> "..Vec2Str(rotAxis).." ("..angImpulse..")");
						self:AddImpulse( -1, hitpos, rotAxis, 100, 1 , {0,0,1}, angImpulse*deathImpulse.rotate);
					end
					lookUp = deathImpulse.chestshot;
				end
				for i,impulse in pairs(lookUp) do
					
					local myPartId = -1;
					if(impulse.partId ~= nil) then
						myPartId = impulse.partId;
					else
						myPartId = self.lastHit.partId;	
					end
					
					local myDirection = {x=0,y=0,z=0};
					if(impulse.use_direction ~= nil)then
						CopyVector(myDirection, self.lastHit.dir);
						FastProductVectors(myDirection,myDirection,impulse.direction);
					else
						CopyVector(myDirection,impulse.direction);
					end
					
					local myImpulse = 0;
					if(impulse.use_strength ~= nil)then
						myImpulse = self.lastHit.damage*0.8*impulse.strength;
					else
						myImpulse = impulse.strength;
					end
					
					--randomize a bit
					myDirection.x = myDirection.x * (math.random()*0.4 + 0.8);
					myDirection.y = myDirection.y * (math.random()*0.4 + 0.8);
					myDirection.z = myDirection.z * (math.random()*0.4 + 0.8);
					
					myImpulse = myImpulse * (math.random()*0.4 + 0.8);
					
					if (self.lastHit.explosion and myImpulse > 120) then
						myImpulse = 120;
					end
					--Log(myPartId.." -> "..Vec2Str(myDirection).." ("..myImpulse..")");
					self:AddImpulse( myPartId, hitpos, myDirection, myImpulse, 1 );
				end
			end
		end		
		
		dir.x = dir.x + math.random()*2 - 1;
		dir.y = dir.y + math.random()*2 - 1;
		dir.z = dir.z + math.random();
		
		NormalizeVector(dir);
		
		CopyVector(self.lastHit.dir,dir);
		self.lastHit.damage = 0;
		self.lastHit.partId = partId;
		self.lastHit.impulse = impulse;-- * 0.5;
		self.lastHit.angImpulse = angImpulse;-- * 0.5;
		
		--self:SetTimer(DEATH_REIMPULSE_TIMER,math.random(300,900));
	end
	
	--Log(self:GetName()..":DeathImpulse("..partId..","..Vec2Str(dir)..","..impulse..")");	
end

function BasicActor:TurnRagdoll(param)
	-- HAX
	if (g_gameRules and g_gameRules:IsMultiplayer() and (not param)) then
		return;
	end

	--AI.LogEvent("BasicActor:TurnRagdoll("..self:GetName()..")");
	--Log("BasicActor:TurnRagdoll("..self:GetName()..")");
	
	self.actor:SetPhysicalizationProfile("ragdoll");
end

function BasicActor:OnPostFreeze(freeze)
	if (freeze and self.actor:IsLocalClient()) then
		System.SetScreenFx("ScreenFrost_CenterAmount", 1);
		System.SetScreenFx("ScreenFrost_Amount", 1);
		self:PlaySoundEvent("sounds/interface:hud:freeze_player", g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
		System.ClearKeyState();
	 	AI.FreeSignal(1, "OnPlayerFrozen", self:GetPos(), 20, self.id);
	elseif (not freeze) then
		if (not self._frost_params) then self._frost_params={}; end;
		self._frost_params.speedMultiplier = 1;
		self.actor:SetParams(self._frost_params);
		
		if (self.actor:IsLocalClient()) then
		 	AI.FreeSignal(1, "OnPlayerUnFrozen", self:GetPos(), 20, self.id);
			System.SetScreenFx("ScreenFrost_Amount", 0);	
			System.SetScreenFx("ScreenFrost_CenterAmount", 0);
		end
	end
	
	-- kill AIs
	if (freeze and not self.actor:IsPlayer()) then
		self:Kill(false, NULL_ENTITY, NULL_ENTITY, freeze);
		AI.ModifySmartObjectStates( self.id, "Dead" );
	end
end


function tf(n) g_gameRules.game:FreezeEntity(g_localActorId or EntityNamed(n).id, true, true); end



function BasicActor:KnockedOutByDoor(hit,mass,vel)
	if(self==g_localActor or self.Properties.species==0 or self:IsDead())then return;end;
	local force=clamp((mass*vel)*0.02,0,100);
	if(force>3)then
		self:PlaySoundEvent("Sounds/physics:mat_wood_hollow:mat_rubble_mixed",{x=0,y=0,z=0.6},g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_PLAYER_FOLEY);
		self:Kill(true, NULL_ENTITY, NULL_ENTITY);
	end;
end;

function BasicActor:Kill(ragdoll, shooterId, weaponId, freeze)

	self.actor:Kill();
	
	self:StopEvent("all");
	
	AI.LogEvent("BasicActor:ClientKill( "..tostring(ragdoll)..", "..tostring(shooterId)..", "..tostring(weaponId));
	if (self.actor:GetHealth() > 0) then
		self.actor:SetHealth(0);
	end

	BasicActor.DoPainSounds(self, 1);
	self:KillTimer(PAIN_TIMER);
	
	self:DropObject();
	local shooter = System.GetEntity(shooterId);
	
	-- send event to music logic
	if (self.MusicInfo) then
		if (self.MusicInfo.headshot == true) then
			MusicLogic.SetEvent(MUSICLOGICEVENT_ENEMY_HEADSHOT);
		elseif (self.MusicInfo.nonbullet == true) then
			MusicLogic.SetEvent(MUSICLOGICEVENT_ENEMY_OVERRUN);
		else
			MusicLogic.SetEvent(MUSICLOGICEVENT_ENEMY_KILLED);
		end;
		self.MusicInfo.headshot = false;
		self.MusicInfo.nonbullet = false;
	end;
	
	-- additional signals for player and squad mates
	if (self == g_localActor) then
		AI.Signal(SIGNALFILTER_GROUPONLY_EXCEPT, 1, "OnPlayerDied", self.id);
		if (self.MusicInfo) then
			MusicLogic.SetEvent(MUSICLOGICEVENT_PLAYER_KILLED);
		end;
	elseif (self.Properties and self.Properties.species == 0) then
	
		-- making player hoslite to squadmates, if he kills one of them
		if(shooter and shooter==g_localActor ) then
			g_SignalData.id = shooter.id;
			AI.Signal(SIGNALFILTER_LEADER,10,"OnUnitBusy",self.id);
			AI.Signal(SIGNALFILTER_GROUPONLY_EXCEPT, 1, "OnPlayerTeamKill", self.id,g_SignalData);
		else	
			AI.Signal(SIGNALFILTER_GROUPONLY_EXCEPT, 1, "OnSquadmateDied", self.id);
		end
		if(self.bUseOrderEnabled and self.useAction and self.useAction>0) then
			if (self.instructionId) then
				HUD:SetInstructionObsolete(self.instructionId);
			end
			self.instructionId = nil;
		end
	--		elseif (hit.shooter and hit.shooter.Properties and hit.shooter.Properties.species ~= 0) then
	--			AI.Signal(SIGNALFILTER_SENDER, 1, "OnEnemyDied", hit.shooter.id);
	end

	-- when a driver ai is dead, something will happen to his vehicle depending on the situation.
	if ( g_gameRules:IsMultiplayer() == false ) then
		if ( self.actor and not self.actor:IsPlayer() ) then
			local vd = self.actor:GetLinkedVehicleId();
			if ( vd ) then
				local ve = System.GetEntity( vd );
				if ( ve ) then
					if ( ve.OnPassengerDead ) then
						ve:OnPassengerDead(self);
					end
				end
			end
		end
	end

	-- Notify CLeader about this
	AI.Signal(SIGNALFILTER_LEADER, 10, "OnUnitDied", self.id);
	AI.Signal(SIGNALFILTER_LEADERENTITY, 10, "OnUnitDied", self.id);
	-- Notify group about this
	g_SignalData.id = self.id;
	CopyVector(g_SignalData.point,self:GetPos());
	
	if (AI.GetGroupCount(self.id) > 1) then
		-- tell your nearest that someone you have died only if you were not the only one
--		AI.Signal(SIGNALFILTER_NEARESTINCOMM, 10, "OnGroupMemberDiedNearest",self.id, g_SignalData); 
		-- notify the closest looking at me dude
		AI.ChangeParameter(self.id,AIPARAM_COMMRANGE,100);
		AI.Signal(SIGNALFILTER_NEARESTINCOMM_LOOKING, 10, "OnBodyFallSound",self.id, g_SignalData);
		-- the closest guy withing 10m will "hear" body falling sound
		AI.ChangeParameter(self.id,AIPARAM_COMMRANGE,10);
		AI.Signal(SIGNALFILTER_NEARESTINCOMM, 10, "OnBodyFallSound",self.id, g_SignalData);
		
--		AI.Signal(SIGNALFILTER_NEARESTINCOMM_LOOKING, 10, "OnGroupMemberDiedNearest",self.id, g_SignalData); 		

	else
		-- tell anyone that you have been killed, even outside your group
		AI.Signal(SIGNALFILTER_ANYONEINCOMM, 10, "OnSomebodyDied",self.id);
	end

	if(shooter) then
		AI.LogEvent("Shooter position:"..Vec2Str(shooter:GetWorldPos()));
		AI.SetRefPointPosition(self.id,shooter:GetWorldPos());	
--		self:InsertSubpipe(0,"DropBeaconAt",shooter.id);
		AI.SetBeaconPosition(self.id, shooter:GetWorldPos());		
	end
		
	-- Call the destructor directly, since the following AIEVENT_TARGETDEAD will
	-- prevent the AI updating further (and kill all signals too).
	if (self.Behaviour and self.Behaviour.Destructor) then
		AI.LogEvent("Calling Destructor for "..self:GetName().." on Kill.");
		self.Behaviour:Destructor(self);
	end
	-- Notify AI system about this
	self:TriggerEvent(AIEVENT_AGENTDIED);
	if (BasicAI) then 
		--Script.SetTimer(1000, BasicAI.OnDeath, self); 
		Script.SetTimerForFunction( 1000 , "BasicAI.OnDeath", self );
	end;
	
	--test death animations
	if (nil) then
		local animStr = "death_death_rifle_right_01";
		local animSpeed = 1.0;
		self:StartAnimation(0, animStr, 0, 0.1, animSpeed, false);
		local wait = self:GetAnimationLength(0,animStr)*1000/animSpeed;
		self:SetTimer(DEADANIM_TIMER,wait*0.99);
	elseif (not self:IsOnVehicle()) then
		if (ragdoll) then
			self:TurnRagdoll(1);
		end	
	end

	if (not self.actor:IsPlayer() and self.Properties.ragdollPersistence>=0 
		and not self:IsOnVehicle() and not freeze) then
		local pos = self:GetPos();
		local level,normal,flow=CryAction.GetWaterInfo(pos);

		if (level and (level-0.8) < pos.z) then
			local ragdollTime = System.GetCVar("g_ragdollMinTime");
			self:SetTimer(DEAD_TIMER, ragdollTime * 1000);
			self:SetTimer(BLOOD_POOL_TIMER,2000);
			self.bodyUnseen = 0;
		end
	end

	if (g_gameRules and g_gameRules.Client.OnPlayerKilled) then
		g_gameRules.Client.OnPlayerKilled(g_gameRules, self);
	end

	return true;	
end


function BasicActor.Client:OnHit(hit)
	if (hit.type=="lockpick") then
		return;
	end

	--Log("BasicActor:ClientOnHit()");
	if (self.hit) then
		self.hit_dir = hit.dir;
	else
		self.hit = true;
		self.hit_dir = hit.dir;
	end

	local shooter = hit.shooter;
		
	-- store some information for musiclogic
	if (not self.MusicInfo) then self.MusicInfo={}; end
	local headshot = g_gameRules:IsHeadShot(hit);
	self.MusicInfo.headshot  = headshot;
		
	self:LastHitInfo(self.lastHit, hit);	
	
	local armor = self.actor:GetArmor();
	
	-- hit effects
	if (string.find(hit.type,"bullet")) then
		if (not self:IsBleeding()) then
			self:SetTimer(BLEED_TIMER, 0);
		end

		local armorEffect = false;
		if(armor > 10) then
			if((hit.material_type ~= "head") or (g_gameRules and g_gameRules:IsMultiplayer())) then
				armorEffect = true;
			end
		end
		
		if(self.id == g_localActorId) then --client player only effects
			local sound;
			if (armorEffect) then
				sound="sounds/physics:bullet_impact:mat_armor_fp";
			else
				sound="sounds/physics:bullet_impact:mat_flesh_fp";
			end
			
			if (sound and (sound~="")) then
				self:PlaySoundEvent(sound, g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
			end
		end
		
		if(tonumber(System.GetCVar("g_useHitSoundFeedback")) > 0) then
			if(hit.shooter and hit.shooter == g_localActor) then
				local sound;
				if (headshot) then
					if(g_gameRules and g_gameRules:IsMultiplayer()) then
						sound="sounds/physics:bullet_impact:headshot_feedback_mp";
					else
						sound="sounds/physics:bullet_impact:headshot_feedback_sp";
						if(hit.target.actor and hit.target.actor:LooseHelmet(hit.dir, hit.pos, true)) then -- helmet takes shot (simulation only)
							--if(not (hit.weapon.class == "DSG1")) then -- sniper rifle ignores helmets
							if(not hit.weapon.weapon:IsZoomed()) then -- zooming ignores helmets
								sound="sounds/physics:bullet_impact:helmet_feedback";
							end
						end
					end
				else
					if(armorEffect) then
						sound="sounds/physics:bullet_impact:generic_feedback";
					else
						if(hit.material_type == "kevlar") then
							sound="sounds/physics:bullet_impact:kevlar_feedback";
						else
							sound="sounds/physics:bullet_impact:flesh_feedback";
						end
					end
				end
				
				if (sound and (sound~="")) then
						self:PlaySoundEvent(sound, g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
				end
			else
				if(self.id == g_localActorId and headshot) then
					if(g_gameRules and g_gameRules:IsMultiplayer()) then
						self:PlaySoundEvent("sounds/physics:bullet_impact:headshot_feedback_fp", g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
					end
				end
			end
		end
		
		if (g_localActor) then
			-- hit particles (additional readability, independent from material system)
			local particleDirection = hit.dir; 
			ScaleVectorInPlace(particleDirection, -1.0);
			local playerPos = g_Vectors.temp_v4;
			g_localActor:GetWorldPos(playerPos);
			local dirVec = playerPos;
			SubVectors(dirVec, playerPos, hit.pos);
			local distance = math.min(50.0, LengthVector(dirVec));
			local distanceScale = 0.2 + (distance / 50.0);			
			if(armorEffect) then
				Particle.SpawnEffect("bullet.hit_flesh.armor", hit.pos, particleDirection, distanceScale);
			else
				Particle.SpawnEffect("bullet.hit_default.smoke", hit.pos, particleDirection, distanceScale*3.0);
			end
		end
	
		self:WallBloodSplat(hit);
		-- marcok: talk to me before touching this
		--if ((self.nextBloodsplat == nil or self.nextBloodsplat < _time) and
		--		not self.actor:IsPlayer() and shooter) then
		--	self.nextBloodsplat = _time + random(3, 4);
			-- re-enabled
			--self:MakeBloodSplats("BloodSplats_Human", 4, hit.pos);
		--end
	end

	local camShakeAmt = tonumber(System.GetCVar("cl_hitShake"));
	local camShakeDuration = 0.35;
	local camShakeFreq = 0.15;

	-- need to save some strength indicator for melees, so we can apply impulse
	-- when we ragdoll them
	if (hit.type=="melee") then
		self.lastMelee = 1;
		self.lastMeleeImpulse = hit.damage * 2;
		
		camShakeAmt = 33;
		--camShakeDuration = 0.5;
		camShakeFreq = 0.2;
	else
		self.lastMelee = nil;
	end
	
	if (self.actor:GetHealth() <= 0) then
		return;
	end
		
	if (self == g_localActor) then
		if (shooter and (self.Properties.species ~= shooter.Properties.species)) then
			g_SignalData.id = shooterId;
			g_SignalData.fValue = 0;
			g_SignalData.iValue = LAS_DEFAULT;
			shooter:GetWorldPos(g_SignalData.point);
			AI.Signal(SIGNALFILTER_SUPERGROUP, 1, "OnPlayerHit", self.id, g_SignalData);
		end	
	end
	
	if (hit.damage > 5 and armor <= 0) then
		if (not self.painSoundTriggered) then
			self:SetTimer(PAIN_TIMER,0.15 * 1000);
			self.painSoundTriggered = true;
		end
	end
	
	--
	self.actor:CameraShake(camShakeAmt, camShakeDuration, camShakeFreq, g_Vectors.v000);
	self.viewBlur = 0.5;
	self.viewBlurAmt = tonumber(System.GetCVar("cl_hitBlur"));
end

function BasicActor:LastHitInfo(dest, hit)
  CopyVector(dest.dir,hit.dir);
	CopyVector(dest.pos,hit.pos);
	dest.partId = hit.partId;
	self:GetVelocity(dest.velocity);
	dest.damage = hit.damage;
	
	if (hit.explosion) then
		dest.explosion = true;
	else
		dest.explosion = false;
	end
	
	if(hit.bulletType) then
		dest.bulletType = hit.bulletType+1;
	else
		dest.bulletType = -1;
	end	
end

function BasicActor:StopSounds()
	self.lastPainSound = nil;
	self.lastPainTime = 0;
end

function BasicActor:DoPainSounds(dead)
	
	-- TODO: Perhaps find alternative solution for the player sounds.
	if (self.actor:IsPlayer()) then

		if (not dead) then		
			if (self.lastPainSound and ((_time-self.lastPainTime<5.0) or Sound.IsPlaying(self.lastPainSound))) then
				return;
			end
		end
		
		local sound;
		if (dead) then
			if(not self.actor:IsLocalClient()) then
				sound = GetRandomSound(self.voiceTable.death_mp);
			else
				sound = GetRandomSound(self.voiceTable.death);
			end
		else
			if(not self.actor:IsLocalClient()) then
				sound = GetRandomSound(self.voiceTable.pain_mp);
			else
				sound = GetRandomSound(self.voiceTable.pain);
			end
		end
		
		if (sound) then
			local sndFlags = bor(bor(SOUND_EVENT, SOUND_VOICE), SOUND_DEFAULT_3D);
			self.lastPainSound = self:PlaySoundEvent(sound[1], g_Vectors.v000, g_Vectors.v010, sndFlags, SOUND_SEMANTIC_PLAYER_FOLEY);
			self.lastPainTime = _time;
		end
		
	else
		-- If a pain sound is still playing, do not trigger new one.
		if (not dead) then
			if (self.lastPainSound and Sound.IsPlaying(self.lastPainSound)) then
				return;
			end
		end
		if (dead) then
			AI.PlayReadabilitySound(self.id, "death", true );
		else
			self.lastPainSound = AI.PlayReadabilitySound(self.id, "pain", true);
		end

	end
	
end

-- Blood splat on player (player is close enough and roughly faces actor)
function BasicActor:MakeBloodSplats(effect, radius, targetPos)
	local blood = tonumber(System.GetCVar("g_blood"));
	if (blood == 0) then
		return;
	end
	
	if (g_localActor:IsOnVehicle() and not self:IsOnVehicle()) then		
		return;
	end
		
	if (targetPos == nil) then
		targetPos = g_Vectors.temp_v2;
		self:GetWorldPos(targetPos);
	end
		
	local playerPos = g_Vectors.temp_v4;
	g_localActor:GetWorldPos(playerPos);
	local dirToTarget = g_Vectors.temp_v1;
	SubVectors(dirToTarget, targetPos, playerPos);
	
	local distance = LengthVector(dirToTarget);
	if (distance < radius) then
		local headDir = g_localActor.actor:GetHeadDir();
		ScaleVectorInPlace(dirToTarget, 1/distance);
		if (dotproduct3d(headDir, dirToTarget)>0.5) then
			local scale = 0.8 - distance/radius;
			scale = scale * scale * 0.3;
			--scale = scale * scale * 0.3;
			-- tone down effect to 10% of original value
--			System.Log("BLOOD: Splat scale="..tostring(scale));
			System.SetScreenFx("BloodSplats_Scale", scale);
			CryAction.ActivateEffect(effect);
			self:PlaySoundEvent("sounds/interface:hud:hud_blood", g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
		end
	end
end


function BasicActor:GetFrozenSlot()
	return 1;
end

----------------------------------------------------------------------------------------------------
function BasicActor:GetFrozenAmount()
	return self.actor:GetFrozenAmount();
end


function BasicActor:SetActorModel(isClient)

	self:KillTimer(UNRAGDOLL_TIMER);
	
	local PropInstance = self.PropertiesInstance;
	local model = self.Properties.fileModel;
	
	-- take care of fp3p
	if (self.Properties.clientFileModel and isClient) then
		model = self.Properties.clientFileModel;
	end
	
	local nModelVariations = self.Properties.nModelVariations;
	if (nModelVariations and nModelVariations > 0 and PropInstance and PropInstance.nVariation) then
	  local nModelIndex = PropInstance.nVariation;
	  if (nModelIndex < 1) then
	  	nModelIndex = 1;
	  end
	  if (nModelIndex > nModelVariations) then
	  	nModelIndex = nModelVariations;
	  end
		local sVariation = string.format('%.2d',nModelIndex);
		model = string.gsub(model, "_%d%d", "_"..sVariation);
		--System.Log( "ActorModel = "..model );
	end
	
	if (self.currModel ~= model) then
		self.currModel = model;	

		self:LoadCharacter(0, model);

		--set all animation events
		self:InitAnimationEvents();

		--set IK limbs
		self:InitIKLimbs();
		
		self:ForceCharacterUpdate(0, false);
		if (self.Properties.objFrozenModel and self.Properties.objFrozenModel~="") then
			self:LoadObject(1, self.Properties.objFrozenModel);
			self:DrawSlot(1, 0);
		end
		
		self:CreateBoneAttachment(0, "weapon_bone", "right_item_attachment");	
		self:CreateBoneAttachment(0, "alt_weapon_bone01", "left_item_attachment");
	
		--laser bone (need it for updating character correctly when out of camera view)
		self:CreateBoneAttachment(0, "weapon_bone", "laser_attachment");	
		
		if(self.CreateAttachments) then
			self:CreateAttachments();
		end
	end	

	if (self.currItemModel ~= self.Properties.fpItemHandsModel) then	
		self:LoadCharacter(3, self.Properties.fpItemHandsModel);
		self:DrawSlot(3, 0);
		self:LoadCharacter(4, self.Properties.fpItemHandsModel); -- second instance for dual wielding
		self:DrawSlot(4, 0);
		
		self.currItemModel = self.Properties.fpItemHandsModel;
	end	
end

function BasicActor:OnNextSpawnPoint()
	local entities = System.GetEntitiesByClass( "SpawnPoint" )
	
	local n = table.getn(entities)
	table.sort( entities, CompareEntitiesByName )

	local nextSpawnPoint = n
	if self.lastSpawnPoint then
		for i = 1, n do
			if self.lastSpawnPoint == entities[i]:GetName() then
				nextSpawnPoint = i
			end
		end
	end
	nextSpawnPoint = nextSpawnPoint + 1
	if nextSpawnPoint > n then
		nextSpawnPoint = 1
	end

	local spawnPoint = entities[nextSpawnPoint]
	self:InternalSpawnAtSpawnPoint(spawnPoint)
end

function BasicActor:SpawnAtSpawnPoint(spawnPointName)
	local entity = System.GetEntityByName(spawnPointName);
	if (entity) then
		local class = System.GetEntityClass(entity.id);
		if (class == "SpawnPoint") then
			self:InternalSpawnAtSpawnPoint(entity);
--			System.Log("BasicActor:SpawnAtSpawnPoint"..entity:GetName());
		end	
	end
end

function BasicActor:InternalSpawnAtSpawnPoint(spawnPoint)
  if (spawnPoint) then
  	self.lastSpawnPoint = spawnPoint:GetName()
  	AI.LogEvent( "Teleport to "..self.lastSpawnPoint )
  
  	self:SetWorldPos(spawnPoint:GetWorldPos(g_Vectors.temp_v1));
  	spawnPoint:GetAngles(g_Vectors.temp_v1);
  	
  	self:SetWorldAngles(g_Vectors.temp_v1);
  	spawnPoint:Spawned(self);
  end
end

function BasicActor:CanGrabObject(object)  
	--FIXME:
	return 0;
	
--  local ret = BasicActor.GrabObject(self, object, 1);
--  if (ret ~= 1) then
--    --GrabLog(self:GetName().." cant grab object "..object:GetName());
--  end
--	return ret or 0;
end

function BasicActor:GrabObject(object, query)
	--FIXME:
	if (query and self.actor:IsPlayer()) then
		return 0;
	end
	
	self.grabParams.entityId = object.id;
	local grabParams = new(self.grabParams);
	grabParams.event = "grabObject";
	
	if (self.actor:CreateCodeEvent(grabParams)) then
		return 1;
	end
	
	return 0;
end

function BasicActor:GetGrabbedObject()
    return (self.grabParams.entityId or 0);
end

function BasicActor:DropObject(throw,throwVec,throwDelay)

	local dropTable =
	{
		event = "dropObject",
		throwVec = {x=0,y=0,z=0},
		throwDelay = throwDelay or 0,
	};
		
	if (throwVec) then
		CopyVector(dropTable.throwVec,throwVec);
	end
		
	self.actor:CreateCodeEvent(dropTable);
end

function BasicActor:IsDead()
	return (self.actor:GetHealth() <= 0);
end

function BasicActor:OnSave(save)
	-- save inventory
	if (self.inventory) then
		save.inventory = self.inventory;
	end
	
end


function BasicActor:OnLoad(saved)
	--if(self.nextHitSound) then
	--	self.nextHitSound = 0;
	--end
end

function BasicActor:OnResetLoad()
	self.actor:SetPhysicalizationProfile("alive");
end

function BasicActor:OnSpawn()
  -- make sure to get a new table  
  self.grabParams = new(self.grabParams);  
  self.waterStats = new(self.waterStats);  
  self.actorStats = new(self.actorStats);
  
  if (CryAction.IsServer()) then
  	if (g_gameRules and g_gameRules.EquipActor) then
			g_gameRules:EquipActor(self);
		end
  end
  
  self.prevFrozenAmount = 0;
end

function BasicActor:Resurrect()
	self:StopBleeding();
	self:OnResetLoad();
	BasicActor.ResetLoad(self);
end

function BasicActor:ScriptEventHide(hide)
end

--function BasicActor:AnimationEvent(event,value)
--	Log("BasicActor:AnimationEvent "..event.." "..value);
--end

function BasicActor:ScriptEvent(event,value,str)
	
	if (event == "cloaking") then
		self:OnCloaking(value);
	elseif (event == "animationevent") then
		if ( self.AnimationEvent ) then
			self:AnimationEvent(str,value);
		end
	elseif (event == "resurrect") then
		self:Resurrect();
	elseif (event == "holster") then
		self:HolsterItem(value);		
	elseif (event == "kill") then
		self:Kill(true, NULL_ENTITY, NULL_ENTITY);
	elseif (event == "fallStart") then
		--Log(self:GetName().." fallStart");
	elseif (event == "landed") then
		if (CryAction.IsServer()) then
			if (g_gameRules and g_gameRules.ProcessFallDamage) then
				g_gameRules:ProcessFallDamage(self, value);
				if(value > 8.0) then
					self:DoPainSounds(false);
				end;
			end
		end
	elseif (event == "detachLadder") then
		if (self.ladderId == self.OnUseEntityId) then
			self:UseEntity( self.OnUseEntityId, self.OnUseSlot, true);
		end
	elseif (event == "splash") then
		if(_time - self.waterStats.lastSplash > 1.0) then
			self.waterStats.lastSplash = _time;
			--PlayRandomSound(self,ActorShared.splash_sounds);
			
			local ppos = g_Vectors.temp_v1;
			self:GetWorldPos(ppos);
			ppos.z = ppos.z + 1;
			
			Particle.SpawnEffect("water.body_splash.enter_water", ppos, g_Vectors.v001, 1.0);
		end
	elseif (event == "jump_splash") then	
		local ppos = g_Vectors.temp_v1;
		self:GetWorldPos(ppos);
		ppos.z = ppos.z + value;
			
		Particle.SpawnEffect("water.body_splash.corpse", ppos, g_Vectors.v001, 1.0);
		if(not str) then
			self:PlaySoundEvent("sounds/physics:player_foley:dive_in", g_Vectors.v000, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_PLAYER_FOLEY);
		end
	elseif (event == "profileChanged") then
		if (str == "ragdoll") then
			self:ApplyDeathImpulse();
		end
	elseif (event == "hide") then
		self:ScriptEventHide(value);		
	elseif (event == "sleep") then
--System.Log(">>>>>>> BasicActor:ScriptEvent:Event_Sleep "..self:GetName());
		-- don't call event_Sleep - it's not inpue-event, not handled with MakeSpawnable
		if(not self.isFallen) then
			BroadcastEvent(self, "Sleep");
		end
		self.isFallen = 1;

--		if(self.Event_Sleep) then
----System.Log(">>>>>>> BasicActor:ScriptEvent:Event_Sleep executing");
--			self:Event_Sleep(self);	
--			self.isFallen = 1;
--		end
	elseif (event == "jumped") then
		local sound = self:PlaySoundEvent("sounds/physics:player_foley:jump_on", g_Vectors.v000, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_PLAYER_FOLEY);
	end
end

function BasicActor.Client:OnAnimationEvent(animation,strPar,intPar)
	
	--function callback, if any
	local onAnimationKey = self.onAnimationKey;
	
	if (onAnimationKey) then
		local func = onAnimationKey[strPar];
		if (func) then
			func(self, animation);
		end
	end
end

function BasicActor.Client:OnTimer(timerId,mSec)
	if (timerId == DEAD_TIMER) then
		local pollTime = System.GetCVar("g_ragdollPollTime");
		if (not BasicActor.RemoveRagdoll(self,mSec*0.001)) then
			self:SetTimer(DEAD_TIMER,pollTime*1000.0);
		end
	elseif (timerId == DEADANIM_TIMER) then
		self.actor:SetPhysicalizationProfile("ragdoll");
	elseif (timerId == DEATH_REIMPULSE_TIMER) then
		self:AddImpulse( self.lastHit.partId, g_Vectors.v000, self.lastHit.dir, self.lastHit.impulse*0.25,1,g_Vectors.v001,self.lastHit.angImpulse*0.25 );
		self:AddImpulse( -1, g_Vectors.v000, self.lastHit.dir, self.lastHit.impulse*0.75,1,g_Vectors.v001,self.lastHit.angImpulse*0.75 );
	elseif (timerId == BLEED_TIMER) then
		if (self:IsBleeding()) then
			self:StopBleeding();
			self:Deplete(true);
		elseif (self.actor:GetHealth()<=0) then
			self:StartBleeding();
			--self:SetTimer(BLEED_TIMER, 60000);
		end
	elseif (timerId == PAIN_TIMER) then
		if (self.actor:GetHealth() > 0) then
			self:DoPainSounds();
		end
		self.painSoundTriggered = nil;
	elseif (timerId == NANOSUIT_MODE_TIMER) then
		self.actor:SetNanoSuitMode(self.AI.NanoSuitMode);
	elseif (timerId == NANOSUIT_CLOAK_TIMER) then
		if(self.AI.NanoSuitCloak) then 
			entity.actor:SetNanoSuitMode(NANOMODE_CLOAK);
		else
			entity.actor:SetNanoSuitMode(NANOMODE_DEFENSE);
		end
	elseif (timerId == COLLISION_TIMER) then
		self.AI.Colliding = false;
	elseif (timerId == ACTOREFFECTS_TIMER) then
		self:UpdateSounds(mSec*0.001);
		--if the actor is dead, dont loop
		if (not self:IsDead()) then
			self:SetTimer(ACTOREFFECTS_TIMER,mSec);
		end
	elseif (timerId == BLOOD_POOL_TIMER) then
	  self:DoBloodPool();
	end
	
end


function BasicActor.Server:OnTimer(timerId, mSec)
	if (timerId == UNRAGDOLL_TIMER) then
		self.actor:SetPhysicalizationProfile("unragdoll");
	end
end


function BasicActor:RemoveRagdoll(frameTime)
	if (not g_localActor) then
		return false;
	end	
	
	local delta = g_Vectors.temp_v1;
	local playerDir = g_Vectors.temp_v2;
	
	self:GetWorldPos(delta);
	g_localActor:GetWorldPos(playerDir);
	
	SubVectors(delta,delta,playerDir);
	
	--if the body is less than "g_ragdollDistance" meters close don't remove it
	local minDist = System.GetCVar("g_ragdollDistance");
	if (LengthSqVector(delta)<minDist*minDist) then
		return false;
	end
	
	NormalizeVector(delta);
	
	g_localActor.actor:GetHeadDir(playerDir);
	
	local dot = dotproduct3d(playerDir,delta);
	
	if (dot < 0) then
		self.bodyUnseen = self.bodyUnseen + frameTime;
	else
		self.bodyUnseen = 0;
	end
	
	--if not visible for "ragdollPersistence" seconds, remove/hide it	
	local ragdollPersistence = self.Properties.ragdollPersistence;
	local ragdollUnseenTime = System.GetCVar("g_ragdollUnseenTime");
	if (ragdollPersistence > 0) then
		ragdollUnseenTime = ragdollUnseenTime * ragdollPersistence;
	end
	
	if (self.bodyUnseen > ragdollUnseenTime) then
		self:RemoveActor();
		return true;
	end
	
	return false;
end

function BasicActor:OnAfterShoot(weapon, pos, dir, impulse, ammo)
	
	self.tempSetStats.inFiring = 10;
	self.actor:SetStats(self.tempSetStats);
		
	local physstats = self:GetPhysicalStats();
	
	if (physstats and (physstats.gravity > -1.0)) then
		local stats = self.actorStats;
		
		--with gravity boots no impulse at all
		if (stats.gravityBoots and stats.onGround > 0) then
			return
		end

		self.impulse_pos = self:GetWorldPos(self.impulse_pos);
		self:AddImpulse(-1, self.impulse_pos, dir, -impulse, 1);
	end
	
	-- registering grenades...
	if (ammo and ammo.class == "ExplosiveGrenade") then
		AI.LogEvent("registering grenades...");
		--self.aiType = 150;
		AI.RegisterWithAI(ammo.id, 150);
	end	
end


function BasicActor:HolsterItem(holster)
--	if(holster) then 
--		AI.LogEvent(self:GetName().." HOLSTERING WEAPON");
--	else
--		AI.LogEvent(self:GetName().." DRAWING WEAPON");
--	end
	self.actor:HolsterItem(holster);
end


function BasicActor:DropItem()
	local item = self.inventory:GetCurrentItemId();
	if (item ~= nil) then
		self.actor:DropItem(item);
	end
end


function BasicActor:NextItem()
	if (ItemSystem) then
		ItemSystem:NextItem(self);
	end
end


function BasicActor:PrevItem()
	if (ItemSystem) then
		ItemSystem:PrevItem(self);
	end
end


--vehicles
function BasicActor:IsOnVehicle()
	return self.actor:GetLinkedVehicleId();
end


-- print entity name of vehicleId, if present
function LogLinkedVehicle(entityName)
  if (entityName) then
    local ent = System.GetEntityByName(entityName);    
    if (ent) then
      if (ent.vehicleId) then
        Log("LinkedVehicle id: "..tostring(ent.vehicleId));
        local vehicle = System.GetEntity(ent.vehicleId);
        if (vehicle) then
          Log("LinkedVehicle: %s", vehicle:GetName());          
        end
      else
        Log("No vehicleId");
      end
    end
  end
end


function BasicActor:ActorLink(entName)
	if (not entName) then
		self.actor:LinkToEntity();		
		self.hideActor = false;	
	else
		local ent = System.GetEntityByName(entName);
		
		self.actor:LinkToEntity(ent.id);
		self.hideActor = true;
	end
end

-------------------------------------------------------------------------
-- state means:
--0 off
--1 chameleon
--2 refraction
--3 temperature
-------------------------------------------------------------------------
function BasicActor:OnCloaking(state)
	
	local stats = self.actorStats;
	
	if (state~=0) then
		if (state == 3) then
			self.camoStartTime = _time;
					
			self:PlaySoundEvent("Sounds/interface:suit:suit_deep_freeze", g_Vectors.v000, g_Vectors.v010, 0, SOUND_SEMANTIC_PLAYER_FOLEY);
			self:PlaySoundEvent("Sounds/interface:suit:breathing_in_mask_cold_oneshot", g_Vectors.v000, g_Vectors.v010, 0, SOUND_SEMANTIC_PLAYER_FOLEY);
						
			if (self.actor:GetChannel()>0) then
				--System.SetScreenFx("ScreenFrost_Amount", 0);
				self.camoFading = true;
				self.camoState = true;
			end
		end

		--Log(self:GetName().." Cloaking::"..state);
		
	else
		local lastState = self.lastCloakState or 1;
		if (lastState == 3) then
			if (self.actor:GetChannel()>0) then
				self.camoFading = true;
				self.camoState = false;
			end
		end

		--Log(self:GetName().." UnCloaking::"..lastState);
	end
	
	self.lastCloakState = state;

	if(state == 1) then
		AI.ChangeParameter(self.id, AIPARAM_CLOAK_SCALE, 1);
	elseif(state == 2) then
		AI.ChangeParameter(self.id, AIPARAM_CLOAK_SCALE, 1);
	elseif(state == 3) then
		AI.ChangeParameter(self.id, AIPARAM_CLOAK_SCALE, 0);
		AI.ChangeParameter(self.id, AIPARAM_HEATSCALE, 0);
	else
		AI.ChangeParameter(self.id, AIPARAM_CLOAK_SCALE, 0);
		AI.ChangeParameter(self.id, AIPARAM_HEATSCALE, 1);
	end

--	AI.ChangeParameter(self.id, AIPARAM_HEATSCALE, stats.heatDamp);
end

function BasicActor:SetCloakType(cloakType)
	local params = 
	{
		nanoSuit =
		{
			--cloaking
			cloakType = 2,
			cloakEnergyCost = 10.0,
			cloakHealthCost = 0.0,
			cloakVisualDamp = 0.5,
			cloakSoundDamp = 0.1,
			cloakHeatDamp = 1.0,
			cloakHudMessage = "normal_cloak",
		},
	};
	
	local nanoSuit = params.nanoSuit;
	if (cloakType and cloakType == 2) then
		nanoSuit.cloakType = 2;
		nanoSuit.cloakEnergyCost = 20.0;
		nanoSuit.cloakHealthCost = 0.0;
		nanoSuit.cloakVisualDamp = 1.0;
		nanoSuit.cloakSoundDamp = 0.5;
		nanoSuit.cloakHeatDamp = 1.0;
		nanoSuit.cloakHudMessage = "alien_cloak";
	elseif (cloakType and cloakType == 3) then
		nanoSuit.cloakType = 3;
		nanoSuit.cloakEnergyCost = 25.0;
		nanoSuit.cloakHealthCost = 3.0;
		nanoSuit.cloakVisualDamp = 0.5;
		nanoSuit.cloakSoundDamp = 0.5;
		nanoSuit.cloakHeatDamp = 0.0;
		nanoSuit.cloakHudMessage = "temperature_cloak";
	end
	
	self.actor:SetParams(params);
end

-----------------------------------------------------------------------------------------------------
function BasicActor:GetDamageImpulseMultiplier()
	return 1
end

--function BasicActor:EntityInFront(entity)
--	Log(entity:GetName());
--end

--test
--[Dejan] I removed this test since it was breaking my code 
--function BasicActor:IsUsable(user)
--	local ret = user:CanGrabObject(self);
--	return ret or 0;
--end
--
--function BasicActor:GetUsableMessage(idx)
--	return "grab";
--end
--
--function BasicActor:OnUsed(user, idx)
--	user:GrabObject(self);
--	return 1;
--end

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

--random debug functions
function SetInertia(inertia,inertiaAccel)

	local params =
	{
		inertia = inertia,
		inertiaAccel = inertiaAccel,
	};
		
	g_localActor.actor:SetParams(params);
	g_localActor.gameParams.inertia = inertia;
	g_localActor.gameParams.inertiaAccel = inertiaAccel;
	g_localActor.actor:PostPhysicalize();
	
	Log("Inertia set for "..g_localActor:GetName()..":"..inertia..","..inertiaAccel);
end

function SetJetpack(thruster,afterburner,thrusterStabilize)

	if (not thruster or not afterburner) then
		Log("Zerog thrusters settings are:"..g_localActor.gameParams.thrusterImpulse..","..g_localActor.gameParams.afterburnerMultiplier);
		return;
	end

	local params =
	{
		thrusterImpulse = thruster,--thruster power, for the moment very related to air_resistance.
		afterburnerMultiplier = afterburner,
		thrusterStabilizeImpulse = thrusterStabilize;--used from the jetpack to make the player to stop.
	};
		
	g_localActor.actor:SetParams(params);
	g_localActor.gameParams.thrusterImpulse = thruster;
	g_localActor.gameParams.afterburnerMultiplier = afterburner;
	g_localActor.gameParams.thrusterStabilizeImpulse = thrusterStabilize;
	
	Log("Zerog thrusters set for "..g_localActor:GetName()..":"..g_localActor.gameParams.thrusterImpulse..","..g_localActor.gameParams.afterburnerMultiplier);
end

function RePhysicalize()

	local p = g_localActor;
	
	p.currModel = "";
	p.actor:SetPhysicalizationProfile("alive");
end

function DumpActorInfo(name)
  local ent = System.GetEntityByName(name);
  if (ent and ent.actor) then
    ent.actor:DumpActorInfo();
  end
end

function TestBeam()

	local beam = System.GetEntityByName("Beam1");
	
	--beam:Activate(1);
	beam:StartAnimation(0, "throw", 0, 0.0, 1.0, false);
		
	g_localActor.actor:CreateCodeEvent({event = "animationEntity",id=beam.id});
	g_localActor.actor:QueueAnimationState("HeavyBeam");
end

function TestLink(entName)
	BasicActor.ActorLink(g_localActor,entName);
	
	if (entName) then
		g_localActor:SetLocalPos({x=0,y=1,z=1});
		g_localActor:SetLocalAngles({x=0,y=0,z=0});
	end
end

function TestCloak(cloakType)
	g_localActor:SetCloakType(cloakType);
end

function TestStances()
	g_localActor.gameParams.stance =
		{
			{
				stanceId = STANCE_STAND,
				normalSpeed = 1.75,
				maxSpeed = 3.5,
				heightCollider = 1.2,
				heightPivot = 1.0,
				size = {x=0.4,y=0.4,z=0.2},
				viewOffset = {x=0,y=0.15,z=0.625},
				modelOffset = {x=0,y=0,z=-1.0},
				name = "combat",
				useCapsule = 1,
			},
			-- -2 is a magic number that gets ignored by CActor::SetupStance
			{
				stanceId = -2,
			},
			--
			{
				stanceId = STANCE_CROUCH,
				normalSpeed = 1.0,
				maxSpeed = 1.5,
				heightCollider = 0.9,
				heightPivot = 0.5,
				size = {x=0.4,y=0.4,z=0.1},
				viewOffset = {x=0,y=0.1,z=0.5},
				modelOffset = {x=0,y=0,z=-0.5},
				name = "crouch",
				useCapsule = 1,
			},
			--
			{
				stanceId = STANCE_PRONE,
				normalSpeed = 0.375,
				maxSpeed = 0.75,
				heightCollider = 0.4,
				heightPivot = 0.175,
				size = {x=0.4,y=0.4,z=0.15},
				viewOffset = {x=0,y=0.5,z=0.175},
				modelOffset = {x=0,y=0,z=-0.175},
				name = "prone",
				useCapsule = 0,
			},
			--
			{
				stanceId = STANCE_SWIM,
				normalSpeed = 1.0,
				maxSpeed = 2.5,
				heightCollider = 0.9,
				heightPivot = 0.5,
				size = {x=0.4,y=0.4,z=0.1},
				viewOffset = {x=0,y=0.1,z=0.5},
				modelOffset = {x=0,y=0,z=0.15},
				weaponOffset = {x=0.3,y=0.0,z=0.3},
				name = "swim",
				useCapsule = 1,
			},
			--
			{
				stanceId = STANCE_ZEROG,
				normalSpeed = 1.75,
				maxSpeed = 3.5,
				heightCollider = 1.2,
				heightPivot = 1.0,
				size = {x=0.4,y=0.4,z=0.3},
				viewOffset = {x=0,y=0.15,z=0.625},
				modelOffset = {x=0,y=0,z=-1},
				weaponOffset = {x=0.3,y=0.0,z=1.3},
				name = "combat",
				useCapsule = 1,
			},
			-- -2 is a magic number that gets ignored by CActor::SetupStance
			{
				stanceId = -2,
			},
		};
end
