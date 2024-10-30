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
Script.ReloadScript("CryMP/Scripts/Entities/AI/Misc/Shark_x.lua");
-----------------------------------------------------------------------------------------------------

CreateActor(Shark_x);
Shark = CreateAI(Shark_x);
Shark:Expose();

----------------------------------------------------------------------------------------------------
function Shark:Event_Spawn()
	-- to do: get target entity from parameter (no g_localActor)
	-- spawn the shark behind the player
	local targetActor = g_localActor;
	self:Event_Enable();
	self.AI.target = targetActor;
	self.actor:SetParams({selectTarget = targetActor.id, spawned = self.AI.spawned});

	self:Hide(0);
	self:Activate(1);

	if(not self.AI.spawned) then -- or self:GetDistance(targetActor.id)>50 or not AI.IsPointInWaterRegion( self:GetPos() )) then
		self.AI.spawned = true;
--		local targetPos = targetActor:GetPos();
--		local targetForw = targetActor:GetDirectionVector(1);
--		targetForw.z = 0;
--		NormalizeVector(targetForw);
--
--		-- position the shark away waiting for the game to reposition it correctly
--		local pos = g_Vectors.temp_v1;
--		FastScaleVector(pos, targetForw, 100);
--		FastSumVectors(pos,pos,targetPos);
--
--		local targetLevel = System.GetTerrainElevation( pos );
--		local targetWaterLevel = AI.IsPointInWaterRegion( pos );
--		if ( targetWaterLevel  > 0.0 ) then
--			targetLevel = targetLevel + targetWaterLevel;
--		end
--		if( pos.z > targetLevel - 2) then
--			pos.z = targetLevel -2;
--		end

		--self:SetWorldPos(pos);
	end

	BroadcastEvent(self, "SpawnShark");
	AI.ModifySmartObjectStates(self.id,"-Escaping");
end;

function Shark:Event_Remove()
	--self.goAwayTime = 4.0;
	self.actor:SetParams({goAway = 1});
	AI.ModifySmartObjectStates(self.id,"Escaping-StickToPlayer");
end

----------------------------------------------------------------------------------------------------
function Shark:Chase(target)
	self.actor:SetParams({targetId = target.id});
	self.AI.target = target;
end

-------------------------------------------------------
function Shark:GetDamageAbsorption()
	return 1;
end

-------------------------------------------------------
function Shark:Event_Disable()
	self:TriggerEvent(AIEVENT_DISABLE);
	self.bNowEnabled = 0;
	--self:Hide(1);
	BroadcastEvent(self, "Disable");
end

-------------------------------------------------------
function Shark:Event_Enable()
	self:TriggerEvent(AIEVENT_ENABLE);
	self.bNowEnabled = 1;
	--self:Hide(0);
	BroadcastEvent(self, "Enable");
end

function Shark:GetDamageAbsorption()
	return 1;
end

Shark.FlowEvents =
{
	Inputs =
	{
		SpawnShark = { Shark.Event_Spawn, "bool" },
		RemoveShark = { Shark.Event_Remove, "bool" },
		DisableShark = { Shark.Event_Disable, "bool" },
		EnableShark = { Shark.Event_Enable, "bool" },
	},
	Outputs =
	{
		Spawn = "bool",
		Remove = "bool",
		Disable = "bool",
		Enable = "bool",
	},
}
