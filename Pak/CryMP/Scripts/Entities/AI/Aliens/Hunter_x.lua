Script.ReloadScript("Scripts/Entities/AI/Shared/BasicAI.lua");
Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicAlien.lua");

HUNTER_GLOW_OFF           = 0;
HUNTER_GLOW_DEAD          = 1;
HUNTER_GLOW_FREEZE_BEAM   = 2;
HUNTER_GLOW_FREEZE_SWEEP  = 3;
HUNTER_GLOW_SINGULARITY   = 4;
HUNTER_GLOW_REGENERATE    = 5;

g_HunterGlowMax = 100; -- this must be changed to 2/4, according to Tiago
g_HunterBaseGlow = 30;  -- dito, percentual

Hunter_x =
{
	AnimationGraph = "Hunter-new.xml",

	voiceType = "hunter",

	foreignCollisionDamageMult = 0.05,
	isAlien = 1,
	isFlying = false,
	isPlayingAnimation = 0,
	--AI related
	PropertiesInstance =
	{
		aibehavior_behaviour = "HunterIdle",
	},
	--AI related	over

	Properties =
	{
	  bExactPos = 0,
		attackrange = 170,
		horizontal_fov = 270,
		accuracy = 1.0,
		species = 2,
		aicharacter_character = "Hunter",
		soclasses_SmartObjectClass = "Hunter",
		fileModel = "Objects/Characters/Alien/Hunter/Hunter.cdf",

		Perception =
		{
			stanceScale = 1.8,
			camoScale = 1,
			FOVsure = 80,
			velScale = 0.1,
			FOV = 160,
			velBase = 1,
			sightrange = 250,
			persistence = 20,
		},

		Damage =
		{
			bLogDamages = 0,
			health = 1000,
			explosionMult = 0.5,
			bulletMult = 0.1,
			playerMult = 1.0,
			AIMult = 0.1,
			torsoMult = 0.5,
			headMult = 1.0,
			legsMult = 0.25,
			weakSpotMult = 2.0,

			HitAccumulator = 5.0,
			HighPoseAccumulator = 1.5,
			HighPoseDuration = 5.0,
		},

		Explosion =
		{
		  effectDelay = 4.5,
		  explosionDelay = 7.5,
		  radius = 15,
		  pressure = 100,
		  damage = 200,
		},

		Vulnerability =
    {
  	  	dmgRatio = 0.1, -- see BasicAlien
  	},
	},

	Vulnerability =
	{
		MatEffects = -- see BasicAlien
		{
			  alien_hunter_torso =
			  {
			      attachments = "vulnerable",
			      radius = 5,
			    	effect="alien_special.Hunter.WoundedPlasmaBody",
			  },
			  alien_hunter_head  =
			  {
			      attachments = "vulnerable",
			      radius = 5,
			    	effect="alien_special.Hunter.WoundedPlasmaBody",
			  },
			  alien_hunter_leg   =
			  {
			      attachments = "vulnerable",
			      radius = 7.5,
			    	effect="alien_special.Hunter.WoundedPlasma",
			  },
			  alien_hunter_vulnerable =
			  {
			      attachments = "weak_point_effect",
			      radius = 1.5,
			      effect="alien_special.Hunter.weakPoint_destroy",
			  },
		},

		Detachables =
		{
			--not sure about keeping these long names
			face_plates_lower_left01_geo =
			{
				attachments =
				{
					"face_plates_lower_left01_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},

				childs =
				{
					"l_mouth02_04_geo",
				},
			},
			face_plates_lower_left02_geo =
			{
				attachments =
				{
					"face_plates_lower_left02_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},

				childs =
				{
					"l_mouth02_05_geo",
				},
			},
			face_plates_lower_right01_geo =
			{
				attachments =
				{
					"face_plates_lower_right01_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},

				childs =
				{
					"r_mouth02_04_geo",
				},
			},
			face_plates_lower_right02_geo =
			{
				attachments =
				{
					"face_plates_lower_right02_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},

				childs =
				{
					"r_mouth02_05_geo",
				},
			},
			face_plates_upper_geo =
			{
				attachments =
				{
					"face_plates_upper_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},

				childs =
				{
					"l_mouth02_01_geo",
					"r_mouth02_01_geo",
				},
			},
			face_plates_upper_left01_geo =
			{
				attachments =
				{
					"face_plates_upper_left01_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			face_plates_upper_left02_geo =
			{
				attachments =
				{
					"face_plates_upper_left02_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},

				childs =
				{
					"l_mouth02_03_geo",
				},
			},
			face_plates_upper_right01_geo =
			{
				attachments =
				{
					"face_plates_upper_right01_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			face_plates_upper_right02_geo =
			{
				attachments =
				{
					"face_plates_upper_right02_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},

				childs =
				{
					"r_mouth02_03_geo",
				},
			},
			l_mouth02_01_geo =
			{
				attachments =
				{
					"l_mouth02_01_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			l_mouth02_02_geo =
			{
				attachments =
				{
					"l_mouth02_02_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			l_mouth02_03_geo =
			{
				attachments =
				{
					"l_mouth02_03_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			l_mouth02_04_geo =
			{
				attachments =
				{
					"l_mouth02_04_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			l_mouth02_05_geo =
			{
				attachments =
				{
					"l_mouth02_05_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			l_mouth02_06_geo =
			{
				attachments =
				{
					"l_mouth02_06_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			r_mouth02_01_geo =
			{
				attachments =
				{
					"r_mouth02_01_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			r_mouth02_02_geo =
			{
				attachments =
				{
					"r_mouth02_02_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			r_mouth02_03_geo =
			{
				attachments =
				{
					"r_mouth02_03_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			r_mouth02_04_geo =
			{
				attachments =
				{
					"r_mouth02_04_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			r_mouth02_05_geo =
			{
				attachments =
				{
					"r_mouth02_05_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
			r_mouth02_06_geo =
			{
				attachments =
				{
					"r_mouth02_06_geo.cgf",
				},
				ratio = 0.05,
				effect =
				{
					"explosions.hunter.plate_A",
					"explosions.hunter.plate_B",
				},
			},
		},
  	},

	physicsParams =
	{
		mass = 100000,

		Living =
		{
			gravity = 9.81,--REMINDER: if there is a ZeroG sphere in the map, gravity is set to 9.81.
						 --It will be fixed, but for now just do all the tweaks without any zeroG sphere in the map.
			mass = 100000,
			inertia = 0,--1.0,
			inertiaAccel = 0,--3.0,
		},
	},

	gameParams =
	{
		stance =
		{
			{
				stanceId = STANCE_CROUCH,
				normalSpeed = 4.8,
				maxSpeed = 6.0,
				heightCollider = 20.0,
				--heightCollider = 0.0,
				heightPivot = 0.0,
				size = {x=1.0,y=1.0,z=0.1},
				--size = {x=0.5,y=0.5,z=0.5},
				--viewOffset = {x=0,y=-7,z=8},
				viewOffset = {x=0,y=6,z=14},
				weaponOffset = {x=0,y=6,z=14},
				modelOffset = {x=0,y=-0,z=0.0},
				name = "high",
				useCapsule = 1,
			},
			{
				stanceId = STANCE_STAND,
				normalSpeed = 4.8,
				maxSpeed = 6.0,
				heightCollider = 17.0,
				--heightCollider = 0.0,
				heightPivot = 0.0,
				size = {x=1.0,y=1.0,z=0.1},
				--size = {x=0.5,y=0.5,z=0.5},
				--viewOffset = {x=0,y=-7,z=8},
				viewOffset = {x=0,y=6,z=8},
				weaponOffset = {x=0,y=6,z=8},
				modelOffset = {x=0,y=-0,z=0.0},
				name = "stand",
				useCapsule = 1,
			},
		},

		speedInertia = 13.0,--the more, the faster the speed change: 1 is very slow, 10 is very fast already
		rotationSpeed = 0.6,
		rollAmount = 0.15,

		sprintMultiplier = 1.0,--speed is multiplied by this ammount when alien is sprinting
		sprintDuration = 0.4,--how long the sprint is

		rotSpeed_min = 0.006 * 1.0,--rotation speed at min speed
		rotSpeed_max = 0.006 * 3.0,--rotation speed at max speed

		speed_min = 0.0,--used by the above parameters

		forceView = 2.0,--multiply how much the view is taking into account when moving

		--grabbing
		maxGrabMass = 1000000,
		maxGrabVolume = 1000000,
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
		damageRadius = 4.0,								-- size of the damage box.
		approachLookat = 0.05,
		alignTime = 0.7,
		damageTime = 1.0,
		meleeDir = {x=0,y=0,z=1};
	},

	grabParams =
	{
		collisionFlags = 0, --geom_colltype_player+geom_colltype0,

		holdPos = {x=-5,y=-17,z=10}, -- default position where the grab is holded.
		--holdPos = {x=0,y=-13,z=6}, -- default position where the grab is holded.

		limbs =
		{
			"frontRightTentacle",
			--"mouthRightTentacle",
		},

		followSpeed = 3.0,
		useIKRotation = 1,

		grabDelay = 1.3,--if IK is used, its the time delay where the limb tries to reach the object.
		--grabDelay = 0.83,--if IK is used, its the time delay where the limb tries to reach the object.

		--FIXME:messy code
		followBone = "frontLegRight12",

		-- NOTE Sep 14, 2007: <pvl> this should be a property of the grabbed entity and not the hunter
		-- but there's no time now to try and track down scripts of everything that the hunter might
		-- possibly grab.
		entityGrabSpot = { 0, 0, 1.0 },

		animation =
		{
			releaseIKTime = 3.0,

			animGraphSignal = "throw",
			forceThrow = 4.65,

			--animGraphSignal = "throwHuman",
			--forceThrow = 6.09,
		},
	},

	IKLimbs =
	{
		{0,"frontRightTentacle","frontLegRight01","","frontLegRight12",0},
		{0,"frontLeftTentacle","frontLegLeft01","","frontLegLeft12",0},
		{0,"backRightTentacle","backLegRight01","","backLegRight13",0},
		{0,"backLeftTentacle","backLegLeft01","","backLegLeft13",0},

		{0,"mouthRightTentacle","frontTentRight01","","frontTentRight16",0},
		{0,"mouthLeftTentacle","frontTentLeft01","","frontTentLeft16",0},
		--{0,"mouthRightTentacle","rope5 seg00","rope5 seg15"},
		--{0,"mouthLeftTentacle","rope6 seg00","rope6 seg15"},
	},


	feetNames =
	{
--		"rope1 seg12",
--		"rope2 seg12",
--		"rope3 seg13",
--		"rope4 seg13"
		"frontLegLeft12",
		"frontLegRight12",
		"backLegLeft13",
		"backLegRight13",
	},

	-- the AI movement ability of the vehicle.
	AIMovementAbility =
	{
		walkSpeed = 4.8,
		runSpeed = 6.0,
		sprintSpeed = 6.0,
		maneuverSpeed = 10.0,
		b3DMove = 0,
		minTurnRadius = 0.1,		-- meters
		maxTurnRadius = 0.5,		-- meters
		pathType = AIPATH_HUNTER,
		pathLookAhead = 18.0,
		cornerSlowDown = 0,
		pathRadius = 6.0,
		passRadius = 9.0,		-- the radius used in path finder.
		resolveStickingInTrace = 0,
		pathRegenIntervalDuringTrace = 0,
	},

	FootStepExplosion =
	{
		Delay 					= 0,
		Effect					= "",
		EffectScale			= 1,
		Radius					= 2,
		Pressure				= 1,
		Damage					= 1000,
		Decal						= "",
		HoleSize				= 0,
		TerrainHoleSize	= 0,
		Direction				= {x=0, y=0, z=1},
	},

  damageTimer = 0,
	pissedOffTime = 0,

	Glow =
	{
	  glowType = HUNTER_GLOW_OFF,
	  baseGlow = g_HunterBaseGlow,
	  glow = 0,
	  dir = 0,
	  goal = 0,
	},

	Server = {},
	Client = {},
}

function InitScriptEvents(table)
  table.scripted_events =
	{
		fireanimation =
		{
			event = ALIENEVENT_SETGRAPHINPUT,
			value = "Signal",
			value2 = "fire",
		},
		presstrigger =
		{
			event = ALIENEVENT_FIREPRIMARY,
			value = 1,
		},
		releasetrigger =
		{
			event = ALIENEVENT_FIREPRIMARY,
			value = 2,
		},
		--FIXME
		faceopen =
		{
			event = ALIENEVENT_CALLFUNCTION,
			func = function(self)
				self:PlaySoundEvent("sounds/Alien:hunter:face_open", {x=0,y=6,z=8}, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_LIVING_ENTITY);
			end,
		},
		faceclose =
		{
			event = ALIENEVENT_CALLFUNCTION,
			func = function(self)
				self:PlaySoundEvent("sounds/Alien:hunter:face_close", {x=0,y=6,z=8}, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_LIVING_ENTITY);
			end,
		},
		glow_charge =
		{
			event = ALIENEVENT_CALLFUNCTION,
			func = HunterStartGlowCharge,
			value = 5,
		},
		glow_discharge =
		{
			event = ALIENEVENT_CALLFUNCTION,
			func = HunterStopGlowCharge,
			value = 1,
		},

		AIShootStart =
		{
			event = ALIENEVENT_CALLFUNCTION,
			func = HunterAIFire,
			value = 1,
		},
		AIShootStop =
		{
			event = ALIENEVENT_CALLFUNCTION,
			func = HunterAIFire,
			value = 0,
		},

		weapon_attack =
		{
			{"fireanimation",0.0},

			{"AIShootStart",0.0},

			{"faceopen",2.7},

			{"presstrigger",3.55},

			{"glow_charge",0.0},

			{"glow_discharge",8.0},

			{"releasetrigger",8.0},

			{"faceclose",9.75},

			{"AIShootStop",14.0},
		},
	};
end

HUNTER_DEATH_EFFECT_TIMER = 202;
HUNTER_DEATH_EXPLOSION_TIMER = 203;

function Hunter_x:CreateAttachments()
	-- create attachment points
	self:CreateBoneAttachment(0, "face_bigass_gun_end", "weapon_effect");
	self:CreateBoneAttachment(0, "face_bigass_gun_end", "right_item_attachment");
	self:CreateBoneAttachment(0, "cockpit", "death_effect_1");

	-- for VS2
	self:CreateBoneAttachment(0, "hunter_attachment",   "vtol_attachment");
end

-----------------------------------------------------------------------------------------------------
function Hunter_x:OnResetCustom()

  self:Hide(0);


	--drop the grabbed entity, if there is one
	self:DropGrab();

	self.foreignCollisionDamageMult = 0.0;

	self.smoothFireDir = {x=0,y=0,z=0};

	--self:ResetMaterial(-1);
	--self:SetAnimationKeyEvent(0,"combat_walk_forward_01",5,11);
	self.deadbodyTouchGround = nil;

	self.Glow.baseGlow = g_HunterBaseGlow;
	self:SetGlow(HUNTER_GLOW_OFF, 0, 0, 0);
	self.SetHunterGlowLevel(self, 0);

	self:SetMaterialFloat(0,6,"glow",50);

	self.isPlayingAnimation = 0;

	self:ResetDetachableSpots();

	--
	self.damageTimer = 0;
	self.pissedOffTime = 0;
	self.isPissed = false;

	self:SetAttachmentEffect(0, "weak_point_effect_01", "alien_special.Hunter.weakPoint", g_Vectors.v000, g_Vectors.v010, 0.5, 0);
	self:SetAttachmentEffect(0, "weak_point_effect_02", "alien_special.Hunter.weakPoint", g_Vectors.v000, g_Vectors.v010, 1, 0);
	self:SetAttachmentEffect(0, "weak_point_effect_03", "alien_special.Hunter.weakPoint", g_Vectors.v000, g_Vectors.v010, 1, 0);

	self:KillTimer(HUNTER_DEATH_EFFECT_TIMER);
	self:KillTimer(HUNTER_DEATH_EXPLOSION_TIMER);

	self:SelectPrimaryWeapon();

	--
	-- pvl dbg stuff
	--self:SetTimer(1000,1000);
	--self.actor:SetAnimationInput( "Stance", "high" )
	--self.stance = "high"
end

--function Hunter_x.Client:OnAnimationEvent(animation,strPar,intPar)
--	Log("animation event on "..animation.." ("..intPar..")");
--end

------------------------------------------------------------------------------
function Hunter_x.Client:OnUpdate(frameTime)

	if (not self:IsDead() and self.pissedOffTime>0.001 and self.isPlayingAnimation<0.01) then
		--Log(self.pissedOffTime);
		self.pissedOffTime = self.pissedOffTime - frameTime;

		if (self.pissedOffTime<0.001) then
			AI.SetStance(self.id,BODYPOS_STAND); --self.actor:SetAnimationInput( "Stance", "stand" );
			--self:StopEverything();
			self.isPissed = false;
		end
	end

	if ( self.isPlayingAnimation ) then
		self.isPlayingAnimation = self.isPlayingAnimation - frameTime;
	end

	if ( self.damageTimer > 0 ) then
		self.damageTimer = self.damageTimer - frameTime;
		if ( self.damageTimer < 0 ) then
			self.damageTimer = 0;
		end
		--System.Log( "self.damageTimer = "..self.damageTimer );
	elseif ( self.damageTimer < 0 ) then
		self.damageTimer = self.damageTimer + frameTime;
		if ( self.damageTimer > 0 ) then
			self.damageTimer = 0;
		end
		--System.Log( "self.damageTimer = "..self.damageTimer );
	end

	if(self.actor:GetHealth()>0) then

		if(not self.LastSoundTime)	 then
			self.LastSoundTime = _time;
		end
		if(_time-self.LastSoundTime > 1) then
			AI.SoundEvent(self:GetWorldPos(g_Vectors.temp), 500, AISE_GENERIC, self.id);
			self.LastSoundTime = _time;
		end
	end
	--FIXME:VS2 hack
	if (self.deadbodyTouchGround) then
		self.deadbodyTouchGround = self.deadbodyTouchGround - frameTime;
		if (self.deadbodyTouchGround<0.0) then
			g_gameRules:ClientViewShake(self:GetWorldPos(),90,60,0.75,0.1);
			self.deadbodyTouchGround = nil;
		end
	end

	self:UpdateGlow(frameTime);

end

------------------------------------------------------------------------------
function Hunter_x:UpdateGlow(frameTime)

  local Glow = self.Glow;
  if (Glow.glowType == HUNTER_GLOW_OFF) then
    return;
  end

  local prevGlow = Glow.glow;
  local goal = Glow.goal;

  if (Glow.dir ~= 0) then
		Glow.glow = Glow.glow + frameTime*Glow.dir;

		if ((prevGlow < Glow.goal and Glow.glow >= Glow.goal) or (prevGlow > Glow.goal and Glow.glow <= Glow.goal)) then
		  self:SetGlow(Glow.glowType, 0, 0, goal);
		  --Log("glow goal %.2f reached", goal);
		end

		if (Glow.glow <= 0 and prevGlow ~= 0) then
		  self:SetGlow(HUNTER_GLOW_OFF, 0, 0, 0);
			--Log("glow 0, reset");
		end
	end

	local newGlow = Glow.glow;
	local phaseAmt = 0;

	-- here goes the type-specific glow control, where needed
	--
	if (HUNTER_GLOW_DEAD == Glow.glowType) then
		-- Flickering Glow fade out after Hunter was killed.
		if (goal ~= 0 and newGlow >= goal) then
			self:SetGlow(HUNTER_GLOW_DEAD, 0, -0.2);
		end
		if (Glow.baseGlow > 0) then
			Glow.baseGlow = __max(0, Glow.baseGlow - 0.2*(frameTime/1000));
		end
		newGlow = newGlow * randomF(0.001,1);

  elseif (HUNTER_GLOW_SINGULARITY == Glow.glowType) then
    -- pulsed charge
    if (goal ~= 0) then
      local freqStart = 2;
      local freqEnd   = 12;
      local freq = freqStart + (newGlow/goal)*(freqEnd-freqStart);
      phaseAmt = goal/freq; -- current amount for one pulse

      if (newGlow < goal-0.5*phaseAmt) then -- pulse until last chargeup is done, then keep glow
        if (phaseAmt < 2*frameTime*Glow.dir) then -- if frequency too high, just turn on/off
          Glow.pulseRatio = (Glow.pulseRatio < 0.5) and 1 or 0;
        else
          local levelRatio = (newGlow%phaseAmt)/phaseAmt;
          Glow.pulseRatio = ((levelRatio < 0.5) and levelRatio or 1-levelRatio)*2; --linear pulse
        end
        newGlow = Glow.pulseRatio * newGlow;
      end
    else
		  self:SetGlow(HUNTER_GLOW_REGENERATE, 0, -1.0);
    end

  elseif (HUNTER_GLOW_REGENERATE == Glow.glowType) then
    if (Glow.glow > 0 and Glow.dir == 0) then
		  self:SetGlow(HUNTER_GLOW_REGENERATE, 0, -1.0);
    end

  elseif (HUNTER_GLOW_FREEZE_BEAM == Glow.glowType) then
    if (Glow.glow > 0 and Glow.dir == 0) then
		  self:SetGlow(HUNTER_GLOW_REGENERATE, 0, -0.1);
    end
	end

	if (newGlow ~= prevGlow) then
	  --Log("Glow = %.3f (phaseAmt %.3f, frameTime %.2f, added %.3f)", newGlow*newGlow, phaseAmt, frameTime, frameTime*Glow.dir);
	  self.SetHunterGlowLevel(self, newGlow*newGlow);
	end
end

------------------------------------------------------------------------------
function Hunter_x:SetGlow(glowtype, goal, speed, initial)
  --Log("SetGlow: %i, %.2f, %.2f", glowtype, goal, speed);
  local Glow = self.Glow;
  Glow.glowType = glowtype;
  Glow.goal = goal;
  Glow.dir  = speed;
  Glow.pulseRatio = 0;
  if (initial) then
    Glow.glow = initial;
  end
end

------------------------------------------------------------------------------
-- Start glowing charge of primary weapon.
function HunterStartGlowCharge( self, seconds )
	--Log("Hunter: *********** GLOW  CHARGE *******");

	local weapon = self.inventory:GetCurrentItem();
	if (weapon) then
  	if (weapon.class == "SingularityCannon") then
  	  self:SetGlow(HUNTER_GLOW_SINGULARITY, 1.0, 0.27);
  	elseif (weapon.class == "MOAR") then
  	  self:SetGlow(HUNTER_GLOW_FREEZE_BEAM, 0.75, 0.4);
  	end
	end
end

------------------------------------------------------------------------------
-- Discharge glow from primary weapon.
function HunterStopGlowCharge( self,seconds )
  --Log("Hunter: StopGlowCharge");
	self:SetGlow(self.Glow.glowType, 0, -0.3);
end


------------------------------------------------------------------------------
-- sets Hunter glowing level from 0 to 1.
function Hunter_x.SetHunterGlowLevel( self, level )
	--Log("Glow = %.3f", level);
	local Glow = self.Glow;

	--if (level == 0) then
		--level = 0.000001;
	--end

	-- todo: Base and Legs used 100*level, Pattern used 5*level
	-- find out if by design or by inconsistent textures
	local val = g_HunterGlowMax * level;

	-- 0: Base, 1: Pattern, 2: Legs
	self:SetMaterialFloat( 0, 0, "glow", Glow.baseGlow + val );
	self:SetMaterialFloat( 0, 1, "glow", val );
	self:SetMaterialFloat( 0, 2, "glow", Glow.baseGlow + val );

end

------------------------------------------------------------------------------
function Hunter_x:OnShoot()

--	self:StopAnimation(0,3);
--	self:StartAnimation(0, "primary_weapon_fire_02", 3, 0.15, 0.75, false,true);
  Log ("Hunter_x:OnShoot() called");
	return 1;
end

------------------------------------------------------------------------------
function Hunter_x:InAir()
	if (self.actorStats.inAir>0.001) then
		return true;
	else
		return false;
	end
end

------------------------------------------------------------------------------
function Hunter_x:CanHoldObject(mass,volume,query)
--	local animState = self.actor:GetCurrentAnimationState();
--	if (animState=="WeaponFire" or animState=="Screaming" or animState=="Throw" or animState=="ThrowHuman") then

	if (self:InAir()) then
		return nil;
	end

	if (self.isPlayingAnimation>0.01) then
		return nil;
	elseif (not query) then
		self.isPlayingAnimation = 9.0;
	end

	return BasicActor.CanHoldObject(self,mass,volume,query);
end

------------------------------------------------------------------------------
function Hunter_x:DoPlayerSeen()

  Log ("Hunter_x:DoPlayerSeen() called");
	BasicAlien.DoPlayerSeen(self);
end

------------------------------------------------------------------------------
function Hunter_x:DoShootWeapon()

  Log ("Hunter_x:DoShootWeapon() called");

	if (self.isPlayingAnimation>0.01) then
		return 0;
	end

	if (self:InAir()) then
		return 0;
	end

--	local animState = self.actor:GetCurrentAnimationState();
--	if (animState=="WeaponFire" or animState=="Screaming" or animState=="Throw" or animState=="ThrowHuman") then
--		return 0;
--	end

	if (not self:IsEventActive("weapon_attack")) then

		self:HolsterItem( false );
		--Log("Hunter_x:DoShootWeapon()");
		self:PushEvent("weapon_attack");

		self.isPlayingAnimation = 13.0;
		return 1;
	end
	return 0;
end

------------------------------------------------------------------------------
function Hunter_x:DoScream()
	if (self.isPlayingAnimation>0.01) then
		return 0;
	end

	if (self:InAir()) then
		return 0;
	end

	if (self.actor:SetAnimationInput( "Signal", "scream" )) then
		self.isPlayingAnimation = 8.0;
		return 1;
	end

	return 0;
end

------------------------------------------------------------------------------
function Hunter_x:StopEverything(stopAnimation)

	do return end;

	--stop firing
	self:StopEvent("all");

	--stop throwing
	local dropTable =
	{
		event = "dropObject",
		throwVec = {x=0,y=0,z=0},
		throwDelay = 0,
		throwImmediately = 1,
	};

	self.actor:CreateCodeEvent(dropTable);

	-- inform AI about this
	self:CancelSubpipe();

	if (stopAnimation) then
		self.actor:SetAnimationInput( "Signal", "stop" );
	end
end

------------------------------------------------------------------------------
function Hunter_x:DoInvestigate()

	self:StopAnimation(0,3);
	self:StartAnimation(0, "fly_idle_break_01", 3, 0.5, 1.0, false,true);
end

------------------------------------------------------------------------------
function Hunter_x:DoPickupTruck()
	self:StopAnimation(0,3);
	self:StartAnimation(0, "truck_arrive", 3, 0.3, 0.5, false,true);
end

------------------------------------------------------------------------------
function Hunter_x:DoHoldTruck()
	self:StopAnimation(0,3);
	self:StartAnimation(0, "truck_idle", 3, 0.15, 1.0, true,true);
end

------------------------------------------------------------------------------
function Hunter_x:DoDropTruck()
	self:StopAnimation(0,3);
	self:StartAnimation(0, "truck_take_off", 3, 0.15, 1.0, false,true);
end

------------------------------------------------------------------------------
function Hunter_x:DoTakeoff()
--	System.Log("hunter-> takeoff");
	self.isFlying = true;
-- 		optimalFlightHeight = 40.0,
--		minFlightHeight = 20.0,
--		maxFlightHeight = 50.0,
	AI.ChangeMovementAbility( self.id, AIMOVEABILITY_OPTIMALFLIGHTHEIGHT, 25 );
	AI.ChangeMovementAbility( self.id, AIMOVEABILITY_MINFLIGHTHEIGHT, 15 );
	AI.ChangeMovementAbility( self.id, AIMOVEABILITY_MAXFLIGHTHEIGHT, 35 );
end

------------------------------------------------------------------------------
function Hunter_x:DoLand()
--	System.Log("hunter-> land");
	self.isFlying = false;
	AI.ChangeMovementAbility( self.id, AIMOVEABILITY_OPTIMALFLIGHTHEIGHT, 10 );
	AI.ChangeMovementAbility( self.id, AIMOVEABILITY_MINFLIGHTHEIGHT, 10 );
	AI.ChangeMovementAbility( self.id, AIMOVEABILITY_MAXFLIGHTHEIGHT, 20 );
end

------------------------------------------------------------------------------
function Hunter_x:TakeOffDropObjectAtPoint(targetPos)
	if(self.grabParams and self.grabParams.entityId and self.grabParams.entityId~= NULL_ENTITY) then
		local grab = System.GetEntity(self.grabParams.entityId);
		if(grab) then
			local objectpos = grab:GetPos();
			local dir = g_Vectors.temp;
			FastDifferenceVectors(dir,targetPos,objectpos);
			local diff = -dir.z;
			NormalizeVector(dir);
			if(diff>5) then
				diff = 0;
			elseif(diff<5) then
				diff = 5-diff;
			end
			local entPos = self:GetPos();
			local newpos = g_Vectors.temp;
			newpos.x = entPos.x + dir.x;
			newpos.y = entPos.y + dir.y;
			newpos.z = entPos.z + diff;
			AI.SetRefPointPosition(self.id,newpos);
--			self:SelectPipe(0,"do_nothing");
			self:SelectPipe(0,"ht_takeoff_and_drop");
		end
	end
end


------------------------------------------------------------------------------
function Hunter_x:GetDamageMultiplier(hit)

	if (self.actor:TrackViewControlled(0)) then
		return 0;
	end

	local mult = 1.0;
	local damageT = self.Properties.Damage;

	if (hit.explosion) then
		mult = mult * damageT.explosionMult;
	else
		mult = mult * damageT.bulletMult;
	end

	local shooter = hit.shooter;
	if (shooter and shooter.actor and shooter.actor:IsPlayer()) then
		mult = mult * damageT.playerMult;
	else
		-- other hurricanes than the player's one dmg hunter 10 times less
		local weapon = hit.weapon;
		if (weapon and (weapon.class == "Hurricane" or weapon.class == "VehicleHurricane")) then
		  mult = mult * 0.1;
		end

		mult = mult * damageT.AIMult;
	end

	local material_type=hit.material_type;
	if (material_type == "alien_hunter_torso") then
	  mult = mult * damageT.torsoMult;
	elseif (material_type == "alien_hunter_head") then
	  mult = mult * damageT.headMult;
	elseif (material_type == "alien_hunter_leg") then
	  mult = mult * damageT.legsMult;
	elseif (material_type == "alien_hunter_vulnerable") then
	  mult = mult * damageT.weakSpotMult;
	end

	if (self:IsSpotVulnerable(material_type, hit.partId)) then
		mult = mult * self.Properties.Vulnerability.dmgMult;
	end

	return mult;
end

------------------------------------------------------------------------------
--FIXME:should be in basicalien?
function Hunter_x:ResetDetachableSpots()
  for i,detachable in pairs(self.Vulnerability.Detachables) do
		detachable.detached = nil;
		detachable.damage = 0;

		for i,attachment in pairs(detachable.attachments) do
      self:HideAttachment(0,attachment,false,false);
      --local filePath = "objects/characters/alien/hunter/"..attachment;
    	--self:SetAttachmentObject(0, attachment, filePath);
	  end
  end
end

------------------------------------------------------------------------------
--FIXME:should be in basicalien?
--function Hunter_x:ProcessDetachableSpots(weaponId, matName, damage, hitType, pos, normal, partId)
function Hunter_x:ProcessDetachableSpots(hit)

  	local closestAttachment = self.actor:GetClosestAttachment(0, hit.pos, 4, "");

  	local detachable = nil;
  	if (closestAttachment) then
  		detachable = self.Vulnerability.Detachables[closestAttachment];
  	end

  	local logDamages = (self.Properties.Damage.bLogDamages == 1);
  	if (detachable and logDamages) then
  	  Log("ProcessDetachableSpots: closest attachment:"..closestAttachment);
  	end

	--local detachable = self.Vulnerability.Detachables[matName.type];

	if (detachable and not detachable.detached) then

		--check if there are childs attached, if so destroy them first.
		if (detachable.childs) then
	      	for i,child in pairs(detachable.childs) do
	      		local childDetachable = self.Vulnerability.Detachables[child];
	      		if (childDetachable and not childDetachable.detached) then
	      			detachable = childDetachable;

	      			if (logDamages) then
	      			  Log("Found detachable part child:"..child.."<-"..closestAttachment);
	      			end
	      		end
			    end
      	end
      	--

		local maxHealth = self.actor:GetMaxHealth();
		local healthRatio = self.actor:GetHealth() / maxHealth;

		detachable.damage = (detachable.damage or 0) + hit.damage;

		local ratio = (detachable.damage / healthRatio) / maxHealth;

		if (logDamages) then
		  Log(closestAttachment.." detachable ratio is "..ratio);
		end

		if (ratio >= detachable.ratio) then
			if (detachable.effect) then
				local effectsNum = table.getn(detachable.effect);
				local effect = detachable.effect[math.random(effectsNum)];

				Particle.SpawnEffect(effect, hit.pos, g_Vectors.v001,1);

				if (logDamages) then
				  Log("Spawning effect:"..effect);
				end
			end

	    	if (detachable.attachments) then
		      	for i,attachment in pairs(detachable.attachments) do

		      		self:HideAttachment(0,attachment,true,true);
		      		--self:ResetAttachment(0, attachment);

		      		if (logDamages) then
		      		  Log("hiding %s", attachment);
		      		end
			    end
	    	end

	    	g_gameRules:ClientViewShake(hit.pos,90,0.1,0.9,0.1);

	    	detachable.detached = 1;
		end
	end
end

function Hunter_x:OnSave (save)
	save.foreignCollisionDamageMult = self.foreignCollisionDamageMult;
	save.smoothFireDir = self.smoothFireDir;
	save.deadbodyTouchGround = self.deadbodyTouchGround;
	save.Glow = self.Glow;
	save.isPlayingAnimation = self.isPlayingAnimation;
	save.damageTimer = self.damageTimer;
	save.pissedOffTime = self.pissedOffTime;
	save.isPissed = self.isPissed;
	save.LastSoundTime = self.LastSoundTime;
	save.deadbodyTouchGround = self.deadbodyTouchGround;
	save.isFlying = self.isFlying;

	save.Detachables = self.Vulnerability.Detachables;

	save.stance = self.stance;
	save.GrabId = self.GrabId;
	save.nextCollCheck = self.Client.nextCollCheck;
	save.firstCrash = self.Client.firstCrash;

	save.grabParams = self.grabParams;
--[[
	self.grabParams.limbs;
	self.grabParams.holdPos;
	self.grabParams.followBone;
	self.grabParams.grabDelay;
	self.grabParams.animation.animGraphSignal;
	self.grabParams.animation.forceThrow;
	self.grabParams.animation.releaseIKTime;
	self.grabParams.animation.grabbedObjOfs;
	self.grabParams.entityGrabSpot;
	self.grabParams.animation.grabbedObjOfs;
--]]

	save.grabbedVTOL = self.grabbedVTOL;
end

function Hunter_x:OnLoad (save)
	self.foreignCollisionDamageMult = save.foreignCollisionDamageMult;
	self.smoothFireDir = save.smoothFireDir;
	self.deadbodyTouchGround = save.deadbodyTouchGround;
	self.Glow = save.Glow;
	self.isPlayingAnimation = save.isPlayingAnimation;
	self.damageTimer = save.damageTimer;
	self.pissedOffTime = save.pissedOffTime;
	self.isPissed = save.isPissed;
	self.LastSoundTime = save.LastSoundTime;
	self.deadbodyTouchGround = save.deadbodyTouchGround;
	self.isFlying = save.isFlying;

	self.Vulnerability.Detachables = save.Detachables;

	self.stance = save.stance;
	self.GrabId = save.GrabId;
	self.Client.nextCollCheck = save.nextCollCheck;
	self.Client.firstCrash = save.firstCrash;

	self.grabParams = save.grabParams;

	self.grabbedVTOL = save.grabbedVTOL;
end

------------------------------------------------------------------------------
function Hunter_x.Server:OnHit(hit)

	--Log("Hunter_x.Server:OnHit");

	if (hit.shooterId == self.id and hit.type ~= "event") then
		return false;
	end

	-- if explosion (ie. no partId/mat passed), get closest part for damage application
	if (hit.explosion) then
		hit.partId, hit.material, hit.material_type = self.actor:GetCloseColliderParts(0, hit.pos, hit.radius);
	end

	local damageMult = self:GetDamageMultiplier(hit);
	if (damageMult<=0) then
		return false;
	end

	self:ProcessHighPose (hit, damageMult)

	local fDamage = hit.damage * damageMult;

	self:ProcessVulnerableSpots(hit);
	self:ProcessDetachableSpots(hit);

	local health = self.actor:GetHealth() - fDamage;
	if (health<1.0) then
		health = 0;
	end

	self.actor:SetHealth(health);

	self:HealthChanged();
	return (health < 1.0);
end

------------------------------------------------------------------------------
function Hunter_x:OnVulnerableHit(partId, materialType)
  if (materialType == "alien_hunter_vulnerable") then
    local glow = self:GetMaterialFloat( 0, 6, "glow" );
    if (glow and glow >= 0.0) then
      if (glow >= 50) then
        glow = 40;
      else
        glow = 0;
      end
      --Log("setting glow to %.2f", glow);
      self:SetMaterialFloat( 0, 6, "glow", glow);
    end
  end
end

------------------------------------------------------------------------------
function Hunter_x.Client:OnTimer(timerId,mSec)
  --Log("hunter OnTimer (%i): %i msec", timerId, mSec);

	if (timerId == HUNTER_DEATH_EFFECT_TIMER) then

		--Log ("Hunter: death effect timer fired")
    local pos = g_Vectors.temp_v1;
    self:GetBonePos("cockpit", pos);
    --Particle.SpawnEffect("alien_special.Hunter.SelfDestruct", pos, g_Vectors.v001, 1);

    self:SetTimer(HUNTER_DEATH_EXPLOSION_TIMER, self.Properties.Explosion.explosionDelay*1000);

  elseif (timerId == HUNTER_DEATH_EXPLOSION_TIMER) then

		--Log ("Hunter: death explosion timer fired")

    --g_gameRules:ClientViewShake(stepPos,70,3.5,0.15,0.095);

    local exp = self.Properties.Explosion;
    local pos = g_Vectors.temp_v1;
    self:GetBonePos("cockpit", pos);

    g_gameRules:CreateExplosion(self.id, self.id, exp.damage, pos, g_Vectors.v001,
      exp.radius, nil, exp.pressure);

    self:Hide(1);

  elseif (timerId == 1000) then
		-- pvl dbg stuff
		--self.actor:SetAnimationInput( "Action", "mrd" )
		--self.actor:SetAnimationInput( "bogus", "5.0" )
		--Log(tostring(self.actorStats.stance));
		if (self.stance == "high") then
			AI.SetStance(self.id,BODYPOS_STAND); --self.actor:SetAnimationInput( "Stance", "stand" )
  		self.stance = "stand"
  	else
  		AI.SetStance(self.id,BODYPOS_CROUCH); --self.actor:SetAnimationInput( "Stance", "high" )
  		self.stance = "high"
  	end
		self:SetTimer(1000,3000);

	else
		BasicActor.Client.OnTimer(self,timerId,mSec);
	end
end

------------------------------------------------------------------------------
function Hunter_x:ProcessHighPose (hit, damageMult)

	if (self.isPlayingAnimation>0.01) then
		return
	end

	local pissedOffDelta = hit.damage * damageMult * 0.066
	self.pissedOffTime = math.min(self.Properties.Damage.HighPoseDuration,self.pissedOffTime + pissedOffDelta);

	--Log ("pissedOffTime="..self.pissedOffTime..", HighPoseAccumulator="..self.Properties.Damage.HighPoseAccumulator)

	--if (self.isPlayingAnimation<0.01 and lastPissedOffTime < 0.001 and self.pissedOffTime > self.Properties.Damage.HighPoseAccumulator) then
	if (self.pissedOffTime >= self.Properties.Damage.HighPoseAccumulator and not self.isPissed) then
		--if (self.actor:SetAnimationInput( "Action", "high" )) then
		AI.SetStance(self.id,BODYPOS_CROUCH); --self.actor:SetAnimationInput( "Stance", "high" );
		--Log(self:GetName().." is pissed!");

		self.pissedOffTime = self.Properties.Damage.HighPoseDuration;

		self.isPlayingAnimation = 1.5;
		self:StopEverything();

		self.isPissed = true;
	end
end

------------------------------------------------------------------------------
function Hunter_x:Kill(ragdoll, shooterId, weaponId)
	BasicAlien.Kill(self,ragdoll,shooterId,weaponId);

	local item = self.inventory:GetCurrentItem();
	if (item) then
		item.item:Quiet();
	end

	--kill the actor
	if (ragdoll) then
		--e3 hack, this should use an animation event
		if (self.isPissed) then
			self:SetTimer(DEADANIM_TIMER,16570);
		else
			self:SetTimer(DEADANIM_TIMER,14200);
		end
		--self.actor:RagDollize();
	end

	self:SetPhysicParams(PHYSICPARAM_SIMULATION, {max_logged_collisions = 1,} );

	-- Notify AI system about this
		self:TriggerEvent(AIEVENT_AGENTDIED);

	-- Start glow fading out on his new cloned material.
	self:SetGlow(HUNTER_GLOW_DEAD, 1.5, 1.0);

	self:ResetAttachment(0, "weak_point_effect_01");
	self:ResetAttachment(0, "weak_point_effect_02");
	self:ResetAttachment(0, "weak_point_effect_03");

	--e3 hack, this should use an animation event
	if (self.isPissed) then
		self.deadbodyTouchGround = 4.53;
	else
		self.deadbodyTouchGround = 2.62;
	end

  --self:SetAttachmentEffect(0, "death_effect_1", "alien_special.Hunter.pre_self_destruct", g_Vectors.v000, g_Vectors.v010, 1, 0);

	--self:SetTimer(HUNTER_DEATH_EFFECT_TIMER, self.Properties.Explosion.effectDelay*1000);
end

------------------------------------------------------------------------------
function Hunter_x.Client:OnHit(hit, remote)

  if (hit.shooterId == self.id) then
		return false;
	end

	local damageMult = self:GetDamageMultiplier(hit);
	if (damageMult<=0) then
		return;
	end

	if (not BasicAlien.Client.OnHit(self,hit,remote)) then
		return false;
	end

	-- make the shooter a bit more important depending on the amount of damage he made
	--AI.UpTargetPriority( self.id, hit.shooterId, 0.2 * hit.damage*damageMult / self.actor:GetHealth() );

	self:ProcessHighPose (hit, damageMult)

	-- react on hits only if huge damage is made in short time interval
	if ( self.damageTimer >= 0 ) then
		self.damageTimer = self.damageTimer + hit.damage * damageMult * 0.066;
		--Log("self.damageTimer:"..self.damageTimer);
	end;

--		-- NOTE Jun 6, 2007: <pvl> request playing of hit reaction anim only if another
--		-- anim is not already playing.  Without this it happened a lot that a hit
--		-- reaction was requested even when another anim (e.g. screaming) was currently
--		-- playing.  The result was that the hit reaction was played only *after*
--		-- the currently playing anim finished, sometimes 5 or 10 seconds after the hunter
--		-- was actually hit.
--		if (self.damageTimer > self.Properties.Damage.HitAccumulator and self.isPlayingAnimation<0.01) then
--
--	--		if (self.actor:SetAnimationInput( "Action", "high" )) then
--	--			self.pissedOffTime = self.Properties.Damage.HighPoseDuration;
--	--		end
--
--			self.damageTimer = self.damageTimer * 0.33;
--			local painSignalName;
--			if ( hit.shooter ) then
--				local shooterDir = {};
--				FastDifferenceVectors( shooterDir, hit.shooter:GetWorldPos(), self:GetWorldPos() );
--				shooterDir.z = 0;
--				NormalizeVector( shooterDir );
--
--				local fwd = self:GetDirectionVector(1);
--				fwd.z = 0;
--				NormalizeVector( fwd );
--
--				local f = dotproduct3d( fwd, shooterDir );
--				if ( f > 0.7071 ) then
--					painSignalName = "damageFront";
--				elseif ( f < -0.7071 ) then
--					painSignalName = "damageBack";
--				else
--					f = fwd.x * shooterDir.y - fwd.y * shooterDir.x;
--					if ( f < 0 ) then
--						painSignalName = "damageRight";
--					else
--						painSignalName = "damageLeft";
--					end
--				end
--			else
--				local damageAnims = {"damageFront","damageLeft","damageRight","damageBack",};
--				painSignalName = damageAnims[math.random(4)];
--			end
--
--			Log(self:GetName().." request pain animation: "..tostring(painSignalName))
--			if (self.actor:SetAnimationInput("Signal",painSignalName)) then
--				self.isPlayingAnimation = 3.5;
--				--self:StopEverything();
--			end
--		end

	if(hit.shooter~=nil) then
		--System.Log( "damageMult = "..damageMult );
		--System.Log( "hit.damage = "..hit.damage );
		g_SignalData.id = hit.shooter.id;
		g_SignalData.fValue = hit.damage * damageMult;
		CopyVector(g_SignalData.point, hit.shooter:GetWorldPos());
		if (self.Properties.species ~= hit.shooter.Properties.species) then
			if(self == g_localActor) then
				-- manage here the player's CLeader since player doesn't have a character/behaviour
				AI.Signal(SIGNALFILTER_LEADER,0,"ORD_ATTACK",self.id,g_SignalData);
			else
				if (self.isVulnerable) then
					self:StopEvent("all");
					self:PushEvent("shield_up");
					AI.Signal(SIGNALFILTER_SENDER,0,"OnSoreDamage",self.id,g_SignalData);
				else
					AI.Signal(SIGNALFILTER_SENDER,0,"OnEnemyDamage",self.id,g_SignalData);
				end
			end
		elseif (self.Behaviour.OnFriendlyDamage ~= nil) then
			AI.Signal(SIGNALFILTER_SENDER,0,"OnFriendlyDamage",self.id,g_SignalData);
		else
			AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
		end
	else
		g_SignalData.id = 0;
		g_SignalData.fValue = damage;
		CopyVector(g_SignalData.point,g_Vectors.v000);
		if (self.isVulnerable) then
			self:StopEvent("all");
			self:PushEvent("shield_up");
			AI.Signal(SIGNALFILTER_SENDER,0,"OnSoreDamage",self.id,g_SignalData);
		else
			AI.Signal(SIGNALFILTER_SENDER,0,"OnDamage",self.id,g_SignalData);
		end
	end
end


--function Hunter_x:Event_GoPatrol( params )
--	AI.Signal(0, 1, "GO_PATROL", self.id);
--end
--
--function Hunter_x:Event_GoSearch( params )
--	AI.Signal(0, 1, "GO_SEARCH", self.id);
--end
--
--function Hunter_x:Event_GoEscort( params )
--	AI.Signal(0, 1, "GO_ESCORT", self.id);
--end

------------------------------------------------------------------------------
function Hunter_x:GrabEntity(entityname)

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

------------------------------------------------------------------------------
function Hunter_x:DropGrab()

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

------------------------------------------------------------------------------
function Hunter_x.Client:OnCollision(hit)

	--Log("scout collide! "..tostring(hit.target_id));
	if (not self.nextCollCheck) then
		self.nextCollCheck = 0;
	end

	if ((not self:IsDead()) or hit.target_id or self.nextCollCheck > _time) then
		return true;
	end

	self.nextCollCheck = _time + 0.3;

	local vel = LengthSqVector(hit.velocity);

	if (vel>10.0) then

		Log("scout collide! square speed:"..vel);

		local pos = g_Vectors.temp_v1;
		CopyVector(pos,self:GetWorldPos());
		pos.z = pos.z + 0.0;

		if (not self.firstCrash) then
			Particle.SpawnEffect("explosions.ScoutCrash.a", hit.pos, g_Vectors.v001,1);
			self.firstCrash = 1;

			g_gameRules:ClientViewShake(pos,90,80,0.9,0.1);
		else
			Particle.SpawnEffect("explosions.ScoutCrashB.a", hit.pos, g_Vectors.v001,randomF(0.5,1.5));

			g_gameRules:ClientViewShake(pos,30,40,0.25,0.09);
		end

		if (self.voiceTable) then
			PlayRandomSound(self,self.voiceTable.crash_on_snow);
		end
	end
end

------------------------------------------------------------------------------
function Hunter_x:ScriptEvent(event,value,str)

	if (event == "liftOff") then
		--self:StopEverything(1);
	elseif (event == "footstep") then
		local stepPos = g_Vectors.temp_v1;
		ZeroVector(stepPos);

		--Log(self.feetNames[value]);
		self:GetBonePos(self.feetNames[value],stepPos);

		--Particle.SpawnEffect("explosions.hunter.footstep",stepPos,g_Vectors.v001,randomF(0.15,0.25));

		local soundEvent = "sounds/Alien:hunter:step";
		sound = Sound.Play(soundEvent, stepPos, SOUND_DEFAULT_3D, SOUND_SEMANTIC_FOOTSTEP);
		local expl = self.FootStepExplosion;

		g_gameRules:CreateExplosion(self.id, self.id, expl.Damage, stepPos, expl.Direction,
			expl.Radius, nil, expl.Pressure, expl.HoleSize, expl.Effect, expl.EffectScale);

		g_gameRules:ClientViewShake(stepPos,70,0.3,0.05,0.095);

	elseif (event == "footlift") then
		local stepPos = g_Vectors.temp_v1;
		ZeroVector(stepPos);

		--Log(self.feetNames[value]);
		self:GetBonePos(self.feetNames[value],stepPos);

		local soundEvent = "sounds/Alien:hunter:leg_servo";
		sound = Sound.Play(soundEvent, stepPos, SOUND_DEFAULT_3D, SOUND_SEMANTIC_LIVING_ENTITY);

	elseif (event == "fireWeapon") then
		self:DoShootWeapon();
	elseif (event == "droppedObject") then
		local grabbed = System.GetEntity(value);
		if (grabbed and grabbed.actor and not grabbed.actor:IsPlayer()) then
			g_gameRules:CreateHit(grabbed.id,self.id,self.id,100000,nil,nil,nil,"event");
		end
	else
		BasicActor.ScriptEvent(self,event,value,str);
	end
end

------------------------------------------------------------------------------
function Hunter_x:AnimationEvent(event,value)
	if ( event == "RegenerateStart" ) then
		self:SetGlow(HUNTER_GLOW_REGENERATE, 1.0, 0.3);
	elseif ( event == "ShootingStart" ) then
		local weapon = self.inventory:GetCurrentItem();
		if (weapon) then
	  	if (weapon.class == "SingularityCannon") then
	  	  self:SetGlow(HUNTER_GLOW_SINGULARITY, 1.0, 0.27);
	  	elseif (weapon.class == "MOAR" or weapon.class == "HunterSweepMOAR") then
	  	  self:SetGlow(HUNTER_GLOW_FREEZE_BEAM, 0.45, 0.25);
	  	end
		end
	elseif ( event == "DeadHitGroundNow" ) then
		self.Client.OnTimer( self, HUNTER_DEATH_EFFECT_TIMER, 0 );
	else
		BasicAI.AnimationEvent(self,event,value);
	end
end

------------------------------------------------------------------------------
function Hunter_x:GetWeaponDir(weapon)
	self.actor:GetHeadDir(self.fireDir);
	--Log(Vec2Str(self.fireDir));
  return self.fireDir;
end

------------------------------------------------------------------------------
function Hunter_x.OnDeath( entity )
	Log(" >>>> Hunter_x.OnDeath "..entity:GetName());
	entity:TriggerEvent(AIEVENT_AGENTDIED);
end

------------------------------------------------------------------------------
function Hunter_x:SetGrabbingScheme(scheme)
	if (scheme and scheme == "Left") then
		self.grabParams.limbs = { "frontLeftTentacle", };
		self.grabParams.holdPos = { x=23, y=-10, z=18 };
		self.grabParams.followBone = "frontLegLeft12";
		self.grabParams.grabDelay = 1.3;
		self.grabParams.animation.animGraphSignal = "throw";
		self.grabParams.animation.forceThrow = 4.65;
		self.grabParams.animation.releaseIKTime = 3.0;
		self.grabParams.animation.grabbedObjOfs = {x=9.69,y=17.75,z=0.0};
	elseif (scheme and scheme == "Mouth") then
		self.grabParams.limbs = { "mouthRightTentacle" }; -- no limbs to prevent crash {"mouthRightTentacle", "mouthLeftTentacle", };
		self.grabParams.holdPos = { x=0, y=-8, z=10 };
		self.grabParams.followBone = "frontTentRight16";
		self.grabParams.grabDelay = 0.83;
		self.grabParams.animation.animGraphSignal = "throwHuman";
		self.grabParams.animation.forceThrow = 6.09;
		self.grabParams.animation.releaseIKTime = 2.0;
		self.grabParams.entityGrabSpot = { x=0, y=0, z=1.0 };
		--self.grabParams.animation.grabbedObjOfs = {x=0.5,y=13.7,z=1.3};
		self.grabParams.animation.grabbedObjOfs = {x=0.5,y=13.7,z=0.0};
	else--Right/default scheme
		self.grabParams.limbs = { "frontRightTentacle", };
		self.grabParams.holdPos = { x=-23, y=-10, z=18 };
		self.grabParams.followBone = "frontLegRight12";
		self.grabParams.grabDelay = 1.3;
		self.grabParams.animation.animGraphSignal = "throw";
		self.grabParams.animation.forceThrow = 4.65;
		self.grabParams.animation.releaseIKTime = 2.0;
		self.grabParams.animation.grabbedObjOfs = {x=9.69,y=17.75,z=0.0};
	end
end

function Hunter_x:GrabObject(object, query)

--	local grabbed = System.GetEntity(object.id);
--	if (grabbed and grabbed.actor and not grabbed.actor:IsPlayer()) then
--		g_gameRules:CreateHit(grabbed.id,self.id,self.id,100000,nil,nil,nil,"event");
--	end

	return BasicActor.GrabObject (self, object, query)
end

------------------------------------------------------------------------------
function HunterAIFire(self,doActivate)
	if (doActivate == 1) then
		AI.ModifySmartObjectStates( self.id, "Shooting" );
		--Log("AIShootStart");
	else
		AI.ModifySmartObjectStates( self.id, "-Shooting" );
		--Log("AIShootStop");
	end

	--self.actor:CreateCodeEvent({event="IKLook",activate=doActivate,});
end

------------------------------------------------------
-- hardcoded VS2 events for vtol grabbing/releasing
------------------------------------------------------
function Hunter_x:Event_GrabVTOL(sender)

  local entName = "VTOL"; -- entity name from CJ
  local ent = System.GetEntityByName(entName);

  if (ent) then
    ent:EnablePhysics(0);
    self:SetAttachmentObject(0, "vtol_attachment", ent.id, -1, 0);
    self.grabbedVTOL = ent;
    Log("Grabbed %s!", entName);
  else
    Log("GrabVTOL: entity %s not found", entName);
  end
end

------------------------------------------------------
function Hunter_x:Event_ReleaseVTOL(sender)
  Log("Event_ReleaseVTOL");
  self:ResetAttachment(0, "vtol_attachment");
  if (self.grabbedVTOL) then
    self.grabbedVTOL:EnablePhysics(1);
  end
end

--TEST FUNCTIONS

------------------------------------------------------
function DetachFaceplates(hunterName)

  local hunter = TestGetHunter(hunterName);

  local nDetached = 0;

 	for i,detachable in pairs(hunter.Vulnerability.Detachables) do
   	if (detachable.attachments) then
    	for i,attachment in pairs(detachable.attachments) do
    		hunter:HideAttachment(0,attachment,true,true);

    		nDetached = nDetached + 1;
    		Log("hiding %s", attachment);
      end
  	end
  end

  Log("%s detached %i faceplates", hunter:GetName(), nDetached);
end

------------------------------------------------------
function TestGetHunter(hunterName)

	if (hunterName) then
		return System.GetEntityByName(hunterName);
	end

	return System.GetEntityByName("Hunter1");
end

------------------------------------------------------
function HunterThrow(hunterName,grabName,scheme)

	local hunter = TestGetHunter(hunterName);

	if (not grabName) then grabName = "Grab1"; end
	local grab = System.GetEntityByName(grabName);

	local throwVec = {};
	local hunterPos = {};
	g_localActor:GetWorldPos(throwVec);
	hunter:GetWorldPos(hunterPos);
	SubVectors(throwVec,throwVec,hunterPos);

	local len = LengthVector(throwVec);
	NormalizeVector(throwVec);

	throwVec.z = throwVec.z - 0.25;
	NormalizeVector(throwVec);

	FastScaleVector(throwVec,throwVec,len);

	hunter:SetGrabbingScheme(scheme);

	Log(Vec2Str(throwVec));
	hunter:GrabObject(grab,nil);
	hunter:DropObject( true, throwVec, 2 );
end

------------------------------------------------------
function HunterFire(hunterName)

	local hunter = TestGetHunter(hunterName);
	hunter:DoShootWeapon();
end

------------------------------------------------------
function HunterScream(hunterName)

	local hunter = TestGetHunter(hunterName);
	hunter:DoScream();
end

------------------------------------------------------
function HunterStop(hunterName)

	local hunter = TestGetHunter(hunterName);
	hunter:StopEverything(1);
end

----------------------------------------------------
function Hunter_x:HealthChanged()

	local lastHealth = self.lastHealth;
	BasicActor.HealthChanged(self);

	local health23 = self.Properties.Damage.health * 0.6667;
	local health13 = self.Properties.Damage.health * 0.3333;
	if ((self.lastHealth <= health23) and (health23 < lastHealth)) then
		self:ActivateOutput("Health_2_3_Left", true);
	elseif ((self.lastHealth <= health13) and (health13 < lastHealth)) then
		self:ActivateOutput("Health_1_3_Left", true);
	end
end

----------------------------------------------------
Hunter_x.FlowEvents =
{
	Inputs =
	{
		GrabVTOL = { Hunter_x.Event_GrabVTOL, "bool" },
		ReleaseVTOL = { Hunter_x.Event_ReleaseVTOL, "bool" },
	},
	Outputs =
	{
		Health_2_3_Left = "bool",
		Health_1_3_Left = "bool",
	},
}

InitScriptEvents(Hunter_x);
