Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicAlien.lua");


AlienPlayer =
{
	type = "Alien",

	Properties =
	{
		health = 255,

		-- AI-related properties
		groupid = 0,
		species = 0,
		commrange = 40; -- Luciano - added to use SIGNALFILTER_GROUPONLY
		-- AI-related properties over

		fileModel = "Objects/characters/alien/methagen/Methagen.cdf",
	},

	physicsParams =
	{
		flags = 0,
		mass = 200,
		stiffness_scale = 73,
		--k_air_control = 0.9, --not used atm

		Living =
		{
			gravity = 15,--REMINDER: if there is a ZeroG sphere in the map, gravity is set to 9.81.
						 --It will be fixed, but for now just do all the tweaks without any zeroG sphere in the map.
			mass = 200,
			--inertia = 5.5, --not used atm
			air_resistance = 0.0, --used in zeroG
		},
	},

	moveParams =
	{
		standSpeed = 7.0,--meters/s
		speedInertia = 9.0,--the more, the faster the speed change: 1 is very slow, 10 is very fast already
		rollAmmount = 3.0,

		sprintMultiplier = 1.5,--speed is multiplied by this ammount when alien is sprinting
		sprintDuration = 0.5,--how long the sprint is

		rotSpeed_min = 0.9 * 7.0,--1.0,--rotation speed at min speed
		rotSpeed_max = 0.6 * 7.0,--rotation speed at max speed

		speed_min = 0.0,--used by the above parameters

		forceView = 1.0,--multiply how much the view is taking into account when moving

		--graphics related
		modelOffset = {x=0,y=0,z=0},
	},

	--melee stuff
	melee_animations =
	{
		{
			"melee_03",
			"melee_04",
			1,
		},
	},

	Server = {},
	Client = {},
}


Net.Expose {
	Class = AlienPlayer,
	ClientMethods = {
		PhysicalizeActor = {RELIABLE_ORDERED, PRE_ATTACH},
	},
	ServerMethods = {
	},
	ServerProperties = {
	}
}

function AlienPlayer.Server:OnInit()
	--self:OnInit();
	self:PhysicalizeActor();
end

function AlienPlayer.Server:OnInitClient( channelId )
	self.onClient:PhysicalizeActor( channelId )
end

function AlienPlayer:PhysicalizeActor()
	BasicAlien.PhysicalizeActor(self);
end

function AlienPlayer.Client:PhysicalizeActor()
	self:PhysicalizeActor()
end

function AlienPlayer.Client:OnInit()
	self:OnInit();
end


function AlienPlayer.Client:OnUpdate( deltaTime )
end

function AlienPlayer.Server:OnUpdate( deltaTime )
	BasicAlien.UpdateServer(self,deltaTime);
end

function AlienPlayer:OnInit()
	self.actor:SetMaxHealth( self.Properties.health )
	self.actor:SetHealth( self.Properties.health )

--	AI.RegisterWithAI(self.id, AIOBJECT_PUPPET, self.Properties, self.PropertiesInstance);
--	self:SetAIName("Alien");

	self:OnReset();
end


function AlienPlayer:OnReset()
--	System.Log(" --->> AlienPlayer:OnReset  -------->>> ");
	BasicAlien.Reset(self);
end

function AlienPlayer:OnAction(action)
	if (action == "attack1") then
		self:MeleeAttack();
	end
end

function AlienPlayer.Client:OnShutDown()
end

function AlienPlayer:OnContact( Entity )
end

function AlienPlayer:OnEvent( EventId, Params )
end

function AlienPlayer:OnSave( props )
end

function AlienPlayer:OnLoad( props )
end

function AlienPlayer.Server:OnTimer()
end

-- return true when the player died
function AlienPlayer:OnHit(hit)

	return false;
end

--function AlienPlayer:Physicalize()
--
--	self:Physicalize(0, PE_LIVING, self.physicsParams);
--	self.actor:SetParams(self.moveParams);
--end
