Script.ReloadScript("Scripts/Entities/AI/Shared/BasicAI.lua");
Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicActor.lua");

Civilian_x = {

------------------------------------------------------------------------------------

	Properties = {
		attackrange = 0,
		species = 10,
		bSpeciesHostility = 0,
		fGroupHostility = 0,
		nVoiceID = 0,
		aicharacter_character = "Hostage",

		fileModel = "",
		objFrozenModel = "",

		voiceType = "enemy",
	},

	gameParams =
	{
		inertia = 0.0, --the more, the faster the speed change: 1 is very slow, 10 is very fast already
		inertiaAccel = 0.0,

		backwardMultiplier = 0.5,--speed is multiplied by this ammount when going backward
	},

	-- the AI movement ability
	AIMovementAbility =
	{
		pathFindPrediction = 0.5,		-- predict the start of the path finding in the future to prevent turning back when tracing the path.
		usePredictiveFollowing = 1,
		walkSpeed = 2.0, -- set up for humans
		runSpeed = 4.0,
		sprintSpeed = 6.4,
		b3DMove = 0,
		pathLookAhead = 1,
		pathRadius = 0.4,
		pathSpeedLookAheadPerSpeed = -1.5,
		cornerSlowDown = 0.75,
		maxAccel = 6.0,
		maneuverSpeed = 1.5,
		velDecay = 0.5,
		minTurnRadius = 0,	-- meters
		maxTurnRadius = 3,	-- meters
		maneuverTrh = 2.0,  -- when cross(dir, desiredDir) > this use manouvering
		resolveStickingInTrace = 1,
		pathRegenIntervalDuringTrace = 4,
	},

	melee =
	{	-- using only damageradius, extended range to detect close threats
		melee_animations =
		{
		},

		damage = 0,			-- damage when doing melee from front
		damageSmall = 0,	-- damage when doing melee from back
		damageOffset = {x=0,y=-2,z=0};		-- Local offset of the damage box
		damageRadius = 20,								-- size of the damage box.
		approachLookat = 0,
		alignTime = 0,
		damageTime = 0,
	},
}


-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------
function Civilian_x:OnResetCustom()
	self.isFree = true;
	self:HolsterItem(true);
	self:ResetOnUsed();
end

-----------------------------------------------------------------------------------------------------
function Civilian_x:SetFree(rescuer)
	AI.Signal(SIGNALFILTER_SENDER,0,"GET_UNTIED", self.id);
end


-----------------------------------------------------------------------------------------------------
--function Civilian_x:GetUsableMessage()
--	return "press USE to free the hostage!";
--end

-----------------------------------------------------------------------------------------------------
function Civilian_x:OnUsed(user)
	BroadcastEvent(self, "Used");
	AI.Signal(SIGNALFILTER_SENDER,1,"USED",self.id);
end

---------------------------------------------
function Civilian_x:Cower()
	if(not self.AI.waiting) then
		if(not self.AI.Cower) then
			self:InsertSubpipe(AIGOALPIPE_NOTDUPLICATE,"hostage_cower");
		else
			self:KillTimer(HOSTAGE_COWER_TIMER);
		end
		self.AI.Cower =true;
		AI.ModifySmartObjectStates(self.id,"Cower");
		self:SetTimer(HOSTAGE_COWER_TIMER,3000);
	end
end

--	if (self.isFree) then
--		return;
--	end
--
--	local rescuePos = g_Vectors.temp_v3;
--	local hostageDir = g_Vectors.temp_v4;
--	--set the player position behind the hostage
--	self:GetWorldPos(rescuePos);
--	self.actor:GetHeadDir(hostageDir);
--
--	user:GetWorldPos(g_Vectors.temp_v2);
--	SubVectors(g_Vectors.temp_v1,rescuePos,g_Vectors.temp_v2);
--
--	local dist = LengthSqVector(g_Vectors.temp_v1);
--
--	if (dist > 3) then
--		return;
--	end
--
--	--check the direction, player must be behind the hostage
--	NormalizeVector(g_Vectors.temp_v1);
--	local dir = dotproduct3d(hostageDir,g_Vectors.temp_v1);
--
--	if (dir < 0.5) then
--		return;
--	end
--
--	if (checking) then
--		return 1;
--	end
--
--	if (not user.hostageID) then
--		user.hostageID = self.id;
--	end
--
--	self.isFree = true;
--
--	rescuePos.x = rescuePos.x - hostageDir.x * 0.9;
--	rescuePos.y = rescuePos.y - hostageDir.y * 0.9;
--
--	user.actor:SetMovementTarget(rescuePos,self:GetWorldPos(),g_Vectors.v000,1);
--
--	user:HolsterItem(true);
--
--	local animName = "interact_hostageUntie_01";
--
--	user:StartAnimation(0, animName, 2, 0.15, 1.0, false);
--	user.actor:SetParams({followCharacterHead = 1,});
--
--	--FIXME:temporary
--	user.stopEPATime = user:GetAnimationLength(0,animName);
--end


-----------------------------------------------------------------------------------------------------
--function Civilian_x:Event_FollowMeHere(sender)
--	AI.Signal(SIGNALFILTER_SENDER,0,"FOLLOW_ME_HERE",self.id,sender:GetWorldPos());
--	BroadcastEvent(self, "FollowMeHere");
--end
-------------------------------------------------------------------------------------------------------
--
