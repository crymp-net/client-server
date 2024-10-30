----------------------------------------------------------------------------------------------------
--  Crytek Source File.
--  Copyright (C), Crytek Studios, 2001-2004.
----------------------------------------------------------------------------------------------------
--  $Id$
--  $DateTime$
--  Description: Basic Alien Functionality
--
----------------------------------------------------------------------------------------------------
--  History:
--  - 20:12:2004   17:35 : Created by Márcio Martins
--  - 					   taken over by Filippo De Luca
----------------------------------------------------------------------------------------------------
Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicActor.lua");
Script.ReloadScript("Scripts/Entities/AI/Shared/BasicAI.lua");

-- Some Alien types (Scouts and Troopers, as of Feb 6, 07) will explode
-- after death with certain probability, otherwise they leave a dead shell.
-- The number of shells of individual Alien types should not be allowed to
-- exceed a predefined limit.  This "class" helps manage these tasks.
--
-- A dead Alien shell will explode with DeadShellCache.explosionProbability.
-- Unexploded shells are stored in DeadShellCache.theShells array where they
-- stay unless a limit on maximum allowable number of dead shells lying
-- around (DeadShellCache.maxNum) is reached.  If that happens, the oldest
-- shell is removed from the array (and the map) with an explosion.
DeadShellCache = {

	AddShell = function (self, actor)
	  --Log("AddShell: probability %.2f", self.explosionProbability);
		if (math.random () > self.explosionProbability) then

			if(not self.theShells) then
				self.theShells = {};
				self.maxNum = 3;
				self.theShells.firstValid = 1;
				self.theShells.lastValid = 1;
			end

			local the_shells = self.theShells;
			the_shells[the_shells.lastValid] = actor;
			the_shells.lastValid = the_shells.lastValid + 1;

			local num_shells = the_shells.lastValid - the_shells.firstValid;
			--Log ("not autodestructing, ".. num_shells .. " so far");
			if (num_shells > self.maxNum) then

				--Log ("cleaning up a rotten shell!");
				local shell_to_remove = the_shells [the_shells.firstValid];
				shell_to_remove:InitiateAutoDestruction();
				the_shells [the_shells.firstValid] = nil;
				the_shells.firstValid = the_shells.firstValid + 1;
				--Log ("... cleaned up, " .. the_shells.lastValid  - the_shells.firstValid .. " remain");
			end
			--Log ("dead shell stored")
			self.bReset = false;
			return true
		end

		--Log ("dead shell refused, will be destructed right away")
		return false
	end,

	Reset = function(self)

		self.bReset = true;

		self.explosionProbability = 0.1;
		self.maxNum = 3;

--		if(not self.theShells) then
			self.theShells = {};
--		end

		self.theShells.firstValid = 1;
		self.theShells.lastValid = 1;

	end,

--	IsReset = function(self)
--		return self.bReset;
--	end,

	--self:Reset();
}


BasicAlien =
{
	AnimationGraph = "Alien.xml",
	UpperBodyGraph = "",

	cloakMaterial = "Materials/Presets/MaterialTypes/Clean/mat_cloak",--"objects/characters/alien/alienbase/basealien_dead",

	BLENDING_RATIO = 1,
	collisionDamageThreshold = 10, -- Amount of collision damage that can be absorbed.
	entityCollisionDamageMult = 0.1, -- default mult for entity/alien collisions
	collisionKillVelocity = 999,

	Properties =
	{
	  	Vulnerability =
	    {
	  	  dmgRatio = 1.0, -- dmg ratio at which spot is considered vulnerable, 1->disabled
	  	  dmgMult  = 1.0, -- multiplier for damage (applied if dmg > dmgRatio)
	    },

	    objFrozenModel = "",
	},

	physicsParams =
	{
		flags = 0,
		mass = 100,
		stiffness_scale = 73,

		Living =
		{
			gravity = 15,--REMINDER: if there is a ZeroG sphere in the map, gravity is set to 9.81.
						 --It will be fixed, but for now just do all the tweaks without any zeroG sphere in the map.
			mass = 800,
			air_resistance = 0.0,

			--FIXME:not fully sure about this
			inertia = 0,
			inertiaAccel = 0,

			k_air_control = 0.9,
		},
	},

	gameParams =
	{
		stance =
		{
			{
				stanceId = STANCE_STAND,
				normalSpeed = 9.5,
				maxSpeed = 19.0,
				heightCollider = 0.0,
				heightPivot = -0.1,
				size = {x=0.6,y=0.6,z=0.5},
				viewOffset = {x=0,y=0,z=0},
				name = "fly",
			},
		},

		speedInertia = 20.0,--the more, the faster the speed change: 1 is very slow, 10 is very fast already
		rollAmount = 0.3, --3.0,
		rollSpeed = 5.0,

		sprintMultiplier = 2.0,--speed is multiplied by this ammount when alien is sprinting
		sprintDuration = 0.4,--how long the sprint is

		rotSpeed_min = 5.0,--rotation speed at min speed
		rotSpeed_max = 8.0,--rotation speed at max speed

		speed_min = 0.0,--used by the above parameters

		forceView = 2.0,--multiply how much the view is taking into account when moving

		--graphics related
		modelOffset = {x=0,y=0,z=0},

		--
		tentaclesMaterial = "mat_alien_flesh",
		tentaclesJointLimit = 10.0,

		--grabbing
		maxGrabMass = 10000,
		maxGrabVolume = 50,--its the square volume of an 1x1x2 meters object
	},

	grabParams =
	{
		holdPos = {x=0,y=-2,z=0}, -- default position where the grab is holded.
	},

	temp_stats =
	{
		idealSpeed = -1,
		blendingRatio = 10,
		approachLookat = 0;
	},

	temp_hit = {},

	Server = {},
	Client = {},

  Vulnerability =
  {
    Spots = {},         -- stores current dmg ratio at vulnerable spots
    MatEffects = {},    -- maps materials to particle/decal names (optional)
    DamageEffects = {}, -- standard health/damage effect/attachment mapping
  },

};

function CreateAlien(child)
	-- Derived class first, the parent classes in backward order.
	mergef(child,BasicAlien,1);
	mergef(child,BasicActor,1);
end


function BasicAlien:Reset()
  --Log("BasicAlien:Reset()");

	BasicActor.ResetCommon(self);

	self.meleeStart = nil;
	self.meleeType = nil;
	self.meleeDamage = nil;

	self:CreateBoneAttachment(0, "weapon_bone", "right_item_attachment");
	--self:CreateBoneAttachment(0, "weapon_bone", "right_item_attachment");

	--play the idle sound in loop
	self:PlayIdleSound(self.voiceTable.idle);

	-- create guard anchor is
	if( self.Properties and self.Properties.bIsGuard == 1 ) then

		-- Create virtual target for the scouts to attack. CXP only!
		local spawnParams = {};
		spawnParams.class = "AIAnchor";
		spawnParams.name = self:GetName().."_guard";
		spawnParams.flags = 0;
		spawnParams.position = {x=0,y=0,z=0};
		spawnParams.orientation = {x=0,y=-1,z=0};
		spawnParams.scale = nil;
		spawnParams.properties = {aianchor_AnchorType = "ALIEN_GUARD",}
		local spawnedEntity = System.SpawnEntity(spawnParams);
		if (spawnedEntity ~= nil) then
			self:AttachChild(spawnedEntity.id, ATTACHMENT_KEEP_TRANSFORMATION);
		end

	end

	-- Check if the initial state should be disabled.
	if( self.Properties.bEnable and self.Properties.bEnable == 0 ) then
		self:Event_Disable();
	end

	self.temp_stats.blendingRatio = BasicAlien.BLENDING_RATIO;
	self.temp_stats.approachLookat = 0;
	self.temp_stats.idealSpeed = -1;
	self.actor:SetParams(self.temp_stats);

	-- reset vulnerability spots and damage effects
	self.Vulnerability.Spots = {};
	self.actor:ResetVulnerabilityEffects(0);
	self:ResetDamageEffects();

	self:KillTimer(PAIN_TIMER);

	--
	self:Cloak(0);
end


function BasicAlien:ResetDamageEffects()
  for i,stage in ipairs(self.Vulnerability.DamageEffects) do
	  self:ResetAttachment(0, stage.attachment);
	end
end

function BasicAlien:TurnRagdoll()
end

function BasicAlien:Kill(ragdoll, shooterId, weaponId)
  self.actor:Kill();

	BasicActor.DoPainSounds(self, 1);
	self:KillTimer(PAIN_TIMER);

	self:DropObject();

	if(shooter) then
		AI.LogEvent("Shooter position:"..Vec2Str(shooter:GetWorldPos()));
		AI.SetRefPointPosition(self.id,shooter:GetWorldPos());
		AI.SetBeaconPosition(self.id, shooter:GetWorldPos());
	end

	self.actor:ResetVulnerabilityEffects(0);

	-- Call the destructor directly, since the following AIEVENT_TARGETDEAD will
	-- prevent the AI updating further (and kill all signals too).
	if (self.Behaviour and self.Behaviour.Destructor) then
		AI.LogEvent("Calling Destructor for "..self:GetName().." on Kill.");
		self.Behaviour:Destructor(self);
	end

	-- Notify CLeader about this
	AI.Signal(SIGNALFILTER_LEADER, 10, "OnUnitDied", self.id);
	-- Notify group about this
	AI.Signal(SIGNALFILTER_SUPERGROUP, 10, "OnGroupMemberDied", self.id);
	local shooter = System.GetEntity(shooterId);

	-- Notify AI system about this
	self:TriggerEvent(AIEVENT_AGENTDIED);

	BasicAlien.StopSounds(self);
	--Script.SetTimer(1000, BasicAlien.OnDeath, self);
	Script.SetTimerForFunction( 1000 , "BasicAlien.OnDeath", self );

	self.actor:SetHealth(0);

	self:StopEvent("all");
end

function BasicAlien.OnDeath(self)
	if (BasicAI and BasicAI.OnDeath) then
		BasicAI.OnDeath(self);
	end
	if(self.OnDeathCustom) then
		self:OnDeathCustom();
	end
end


function BasicAlien.Server:OnHit(hit)

	--AI.LogEvent("BasicAlien.Server:OnHit(damage:"..damage..",to:"..self:GetName()..")");

	--FIXME:this should be saved together with all the other lastHit varibales, in Client:OnHit, but at the moment explosion infos dont get there.
	self.lastHit.explosion = hit.explosion;

	local damage=hit.damage;
	if(self.Properties.damageScale) then
		damage = hit.damage * self.Properties.damageScale;
	end

	local multiplier = self:GetDamageMultiplier(hit);
	damage = damage*multiplier;

	local health = math.floor(self.actor:GetHealth() - damage);

	--Log("<%s> damage %.1f / newHealth %.0f", self:GetName(), damage, health);

--	if (health<0.99) then
--		health = 0;
--	end
	self.actor:SetHealth(health);

	self:HealthChanged();
	return (health <= 0);
end

------------------------------------------------------------------------------
function BasicAlien.Client:OnHit(hit, remote)

  local health = self.actor:GetHealth();

	if (health <= 0) then
		return false;
	end

	local damageMult = self:GetDamageMultiplier(hit);
	local damage = hit.damage * damageMult;

	if (self.hit) then
		self.hit_dir = hit.dir;
	else
		self.hit = true;
		self.hit_dir = hit.dir;
	end

	CopyVector(self.lastHit.dir,hit.dir);
	CopyVector(self.lastHit.pos,hit.pos);
	self.lastHit.partId = hit.partId;
	self:GetVelocity(self.lastHit.velocity);

	self:AddImpulse(hit.partId,hit.pos,hit.dir,hit.damage *  self:GetDamageImpulseMultiplier(hit),2);

	-- attach optional damage effects
	if (damage > 0) then
	  local maxHealth = self.actor:GetMaxHealth();
	  local oldRatio = health/maxHealth;
	  local newRatio = __max(0, (health-damage)/maxHealth);

	  if (newRatio ~= 0) then
  	  for i,stage in ipairs(self.Vulnerability.DamageEffects) do
  	    if (oldRatio >= stage.health and newRatio < stage.health) then
  	      --Log("<%s>: ratio %f: setting effect %s on attachment %s", self:GetName(), newRatio, stage.effect, stage.attachment);
  	      self:SetAttachmentEffect(0, stage.attachment, stage.effect, g_Vectors.v000, g_Vectors.v010, 1, 0);
  	    end
  	  end
  	end
	end

	--BasicActor.DoPainSounds(self);
	if (not self.painSoundTriggered) then
		self:SetTimer(PAIN_TIMER,0.15 * 1000);
		self.painSoundTriggered = true;
	end

	if (string.find(hit.type,"bullet")) then
		if(hit.shooter and hit.shooter == g_localActor) then
			if(tonumber(System.GetCVar("g_useHitSoundFeedback")) > 0) then
				if(hit.target and hit.target.class == "Alien") then
					self:PlaySoundEvent("sounds/physics:bullet_impact:alien_flesh_feedback", g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
				else
					self:PlaySoundEvent("sounds/physics:bullet_impact:alien_shell_feedback", g_Vectors.v000, g_Vectors.v010, SOUND_2D, SOUND_SEMANTIC_PLAYER_FOLEY);
				end
			end
		end
	end

	return true;
end

------------------------------------------------------------------------------
function BasicAlien:GetDamageMultiplier(hit)
  return 1;
end


function BasicAlien:PlayIdleSound(idlesound)

	local sound = GetRandomSound(idlesound);

	if (not sound) then
		return;
	end

	local offset;
	if (sound[2]) then
		offset = sound[2];
	else
		offset= g_Vectors.v000;
	end
--		local gp = self.gameParams;
--		if (gp and gp.stance) then
----			local stanceIdx = AI.GetStance(self.id);
----			if(not stanceIdx) then
----				stanceIdx = 1;
----			end
--			local curStance= gp.stance[1];
--			if(curStance) then
--				offset = curStance.viewOffset;
--			end
--		end
--		if(not offset) then
--			offset= g_Vectors.v000;
--		end
--	end

	--stop the current idle
	if (self.idleSound) then
		self:StopSound(self.idleSound);
	end

	local sndFlags = bor(SOUND_DEFAULT_3D,SOUND_LOOP);
	self.idleSound = self:PlaySoundEvent(sound[1], offset, g_Vectors.v010, sndFlags, SOUND_SEMANTIC_LIVING_ENTITY);
	--Sound.SetSoundLoop(self.idleSound,1);
end

function BasicAlien:StopSounds()
	BasicActor.StopSounds(self);

	if (self.idleSound) then
		self:StopSound(self.idleSound);
	end

	self.idleSound = nil;
end

function BasicAlien:GetDamage()
	-- used for melee attack
	return 0;
end

function BasicAlien:MeleeAttack(meleeType)
	if (not self.meleeStart) then
		AI.LogEvent("meelee attack!!");

		local attacksNum = table.getn(self.melee.melee_animations);
		local attack = self.melee.melee_animations[math.random(1,attacksNum)];

		local num = table.getn(attack);
		local aidx = 1;

		if (num>2) then
			aidx = attack[num];

			attack[num] = attack[num] + 1;
			if (attack[num]>num-1) then
				attack[num] = 1;
			end
		end

--		self.actor:QueueAnimationState(attack[aidx]);
		if(self.Readibility) then
			self:Readibility("MELEE",1,10);		--priority 10
		end

		--play tentacle sounds
--		local sound = GetRandomSound(self.voiceTable.tentacles);
--		if (sound) then
--			self:PlaySoundEvent(sound[1], g_Vectors.v000, g_Vectors.v010, SOUND_DEFAULT_3D, SOUND_SEMANTIC_WEAPON);
--		end

		self.meleeStart = _time;

		if (not meleeType) then
			self.meleeType = 0;
		else
			self.meleeType = meleeType;
		end

		--
		self.temp_stats.idealSpeed = self.gameParams.stance[1].maxSpeed * 0.33;
		self.temp_stats.blendingRatio = 60;
		self.temp_stats.approachLookat = 0;

		self.actor:SetParams(self.temp_stats);

	end
end

function BasicAlien.Server:OnUpdate(frameTime)
	BasicActor.Server.OnUpdate(self,frameTime);

	if (not self:IsDead()) then
		self:UpdateMelee(frameTime);
	end
end

function BasicAlien:UpdateMelee(frameTime)
	if (self.meleeStart) then
		local dt = _time-self.meleeStart;

		if( (dt < self.melee.alignTime) ) then
			-- Align the alien towards its' target.
			self.temp_stats.approachLookat = self.melee.approachLookat;
			self.actor:SetParams(self.temp_stats);
		elseif( dt < self.melee.damageTime ) then
			if (not self.meleeDamage) then
				self.meleeDamage = true;
				self:MeleeDamage();
			end
			self.temp_stats.approachLookat = 0;
			self.actor:SetParams(self.temp_stats);
		else
			self.temp_stats.idealSpeed = -1;	--disable ideal speed
			self.temp_stats.blendingRatio = BasicAlien.BLENDING_RATIO;
			self.temp_stats.approachLookat = 0;

			self.actor:SetParams(self.temp_stats);

			self.meleeStart = nil;
			self.meleeDamage = nil;
		end
	end
end

function BasicAlien:MeleeDamage()
	AI.LogEvent( "BasicAlien:MeleeDamage()" );
	local headDir = self.actor:GetHeadDir();
	local pos = self:GetWorldPos();
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

	local radius = self.melee.damageRadius;
	local	offset = self.melee.damageOffset;

	pos.x = pos.x + dirX.x * offset.x + dirY.x * offset.y + dirZ.x * offset.z;
	pos.y = pos.y + dirX.y * offset.x + dirY.y * offset.y + dirZ.y * offset.z;
	pos.z = pos.z + dirX.z * offset.x + dirY.z * offset.y + dirZ.z * offset.z;

--	pos.x = pos.x + headDir.x*radius*0.75;
--	pos.y = pos.y + headDir.y*radius*0.75;
--	pos.z = pos.z + headDir.z*radius*0.75;

	local entities = System.GetPhysicalEntitiesInBox(pos, radius);

	g_gameRules:ClientViewShake(pos,10,20,0.25,0.09);

	if (entities) then
		-- calculate damage for each entity
		for i,entity in ipairs(entities) do

			local sameSpecies = false;
			if( self.Properties.species and entity.Properties.species ) then
				if( self.Properties.species == entity.Properties.species ) then
					sameSpecies = true;
				end
			end

			if (entity ~= self and not sameSpecies) then

				-- Hit!
				if (g_gameRules and g_gameRules.server and g_gameRules.server.RequestMeleeHit) then

					local hit = self.temp_hit;
					AI.LogComment( "BasicAlien:MeleeDamage: Hit "..entity:GetName().."." );

					hit.pos = pos;
					hit.partId = 0;
					hit.dir = hitDir;
					hit.shooter = self;
					hit.weapon = self;
					hit.type 		= "melee";
					if( self.meleeType == 0 ) then
						hit.damage	= self.melee.damage;
					else
						hit.damage	= self.melee.damageSmall;
					end

					hit.target	= entity;

					hit.normal = hit.normal or {};
	        CopyVector(hit.normal, hitDir);
	        NegVector(hit.normal);

					--g_gameRules.server:RequestMeleeHit(hit.shooter.id, hit.shooter.id, hit.target.id, 0, hit.pos);
					g_gameRules.Client.OnHit(g_gameRules, hit,false);
				end

				self:MeleeCustom(entity,pos,hitDir);

			end
		end
	else
		AI.LogEvent( "BasicAlien:MeleeDamage: no entities on radius "..radius.."." );
	end
end

function BasicAlien:MeleeCustom(entity,pos,dir)
	-- push target away

	local shake = 20;

	if( self.meleeType == 0 ) then
		entity:AddImpulse(-1, pos, dir, 500, 1);
	else
		dir.y = dir.y + 0.2;
		NormalizeVector(dir);
		entity:AddImpulse(-1, pos, dir, 2500, 1);
		shake = 100;
	end

	if( entity.actor ) then
		entity.actor:CameraShake(shake, 0.5, 0.1, g_Vectors.v000);
		entity.actor:AddAngularImpulse({x=randomF(-math.pi*0.01,math.pi*0.01),y=0,z=-math.pi*0.1},0.0,1.5);
	end
end

function BasicAlien:DoPlayerSeen()

	if (self.voiceTable) then
		PlayRandomSound(self,self.voiceTable.player_seen);
	end
end

function BasicAlien:BlendAnimation(factor)
	self.actor:SetParams({blendingRatio = factor});
end

function BasicAlien:ShutDown()
	self:ResetAttachment(0, "right_item_attachment");
	self:StopSounds();
end

function BasicAlien:Cloak(cloak)

	local cloaked = (self.cloaked or 0);

	if (cloaked ~= (cloak or 0)) then

		local newMat = nil;
		if (cloaked == 0) then
			newMat = self.cloakMaterial;
		end

		self.cloaked = (cloak or 0);

		self.actor:CreateCodeEvent({event = "replaceMaterial",material=newMat,cloak=self.cloaked});

		if (self.cloaked == 1) then
			AI.ChangeParameter(self.id,AIPARAM_CAMOSCALE,0.1);
			self:PlayIdleSound(self.voiceTable.idleCloak);
		else
			AI.ChangeParameter(self.id,AIPARAM_CAMOSCALE,1);
			self:PlayIdleSound(self.voiceTable.idle);
		end
		if(cloak==1) then
			AI.ChangeParameter(self.id,AIPARAM_CAMOSCALE,0.1);
			self:Readibility("CLOAK",1,100);

		else
			AI.ChangeParameter(self.id,AIPARAM_CAMOSCALE,1);
			self:Readibility("UNCLOAK",1,100);
		end
	end

end

function BasicAlien:ScriptEventHide(hide)
	if(hide == 1) then
		-- hide
		self:StopSounds();
	else
		-- unhide
		if (self.cloaked == 1) then
			AI.ChangeParameter(self.id,AIPARAM_CAMOSCALE,0.1);
			self:PlayIdleSound(self.voiceTable.idleCloak);
		else
			AI.ChangeParameter(self.id,AIPARAM_CAMOSCALE,1);
			self:PlayIdleSound(self.voiceTable.idle);
		end
	end
end


-------------------------------------------------------------------------------------------------------
function BasicAlien:OnAlert(sender)
	if (sender) then
		AI.LogEvent(self:GetName()..": receiving EVENT ALERT sent by entity "..sender:GetName());
		g_SignalData.ObjectName = sender:GetName();
		CopyVector(g_SignalData.point, sender:GetPos());
		AI.Signal(SIGNALFILTER_SENDER, 10, "OnAlertReceived", self.id, g_SignalData);
	else
		AI.LogEvent(self:GetName()..": invoking EVENT ALERT");
	end
	BroadcastEvent(self, "Alert");
end

-------------------------------------------------------------------------------------------------------
function BasicAlien:IsSpotVulnerable(material_type, partId)
  local spots = self.Vulnerability.Spots;

  if (not spots[partId]) then
    return false;
  elseif (spots[partId] > self.Properties.Vulnerability.dmgRatio) then
		return true;
	else
		return false;
	end
end


-------------------------------------------------------------------------------------------------------
function BasicAlien:ProcessVulnerableSpots(hit)

  -- this accumulates damage points for vulnerability spots
  -- it's only executed if Properties.Vulnerability.dmgRatio < 1
  if (hit.partId and hit.partId > -1) then
    if (self.Properties.Vulnerability.dmgRatio < 1) then

	    -- count damage for spot (as fraction of max health)
	    local prevRatio, newRatio;
	    local ratio = hit.damage/self.actor:GetMaxHealth();
	    local spots = self.Vulnerability.Spots;

	    prevRatio = spots[hit.partId] or 0;
	    newRatio = prevRatio + ratio;
	    spots[hit.partId] = newRatio;
	    --Log("part %i -> ratio %f", hit.partId, newRatio);

	    local props = self.Properties.Vulnerability;
	    if (newRatio > props.dmgRatio and prevRatio <= props.dmgRatio) then

	      --Log("vulnerability ratio exceeded on partId %i (material %s)", hit.partId, hit.material_type or "<NULL>");

	      local matEff = self.Vulnerability.MatEffects[hit.material_type];

	      -- attach particle, if set
	      if (matEff and matEff.effect) then
          local attName = self.actor:AttachVulnerabilityEffect(0, hit.partId, hit.pos, matEff.radius, matEff.effect, matEff.attachments);
    	  end

  	    -- create decal, if set
	      if (matEff and matEff.decal and hit.normal) then
	        Particle.CreateDecal(hit.pos, hit.normal, 1, 300, matEff.decal, nil, nil, self.id, hit.partId);
	      end

	      if (self.OnVulnerableHit) then
	        self:OnVulnerableHit(hit.partId, hit.material_type);
	      end
	    end
	  end
	end
end

-----------------------------------------------------------------------------------------------------
function BasicAlien:GetDamageImpulseMultiplier()
	return 1
end

-----------------------------------------------------------------------------------------------------
function BasicAlien:Event_Cloak(sender)
	self:Cloak(1);
	BroadcastEvent(self, "Cloak");
end

function BasicAlien:Event_UnCloak(sender)
	self:Cloak(0);
	BroadcastEvent(self, "UnCloak");
end

-----------------------------------------------------------------------------------------------------
function BasicAlien:Event_EnableSearchBeam(sender)
	if (self.EnableSearchBeam) then
	  self:EnableSearchBeam(true);
	end
end

--------------------------------------------------------------------------------------------------------
function BasicAlien:Event_Enable(params)
	if (not self:IsDead() ) then
		-- hide does enable/disable physics as well
		self:Hide(0)
		self:Event_Enabled(self);
		if(self.voiceTable) then
			if (self.cloaked == 1 and self.voiceTable.idleCloak) then
				self:PlayIdleSound(self.voiceTable.idleCloak);
			elseif(self.voiceTable.idle) then
				self:PlayIdleSound(self.voiceTable.idle);
			end
		end
	end
end


BasicAlien.FlowEvents =
{
	Inputs =
	{
	  Cloak = { BasicAlien.Event_Cloak, "bool" },
	  UnCloak = { BasicAlien.Event_UnCloak, "bool" },
	  EnableSearchBeam = { BasicAlien.Event_EnableSearchBeam, "bool" },
	},
	Outputs =
	{
	  Cloak = "bool",
	  UnCloak = "bool",
	},
}


--function TrooperJump()
--	System.GetEntityByName("Trooper1"):JumpTo(System.GetEntityByName("TagPoint3"):GetWorldPos());
--end

function AlienWeapon(name, index)
  local ent = System.GetEntityByName(name);
  if (index == 0) then
    ent:SelectPrimaryWeapon();
  elseif (index == 1) then
    ent:SelectSecondaryWeapon();
  end
end
