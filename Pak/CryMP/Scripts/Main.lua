function OnInit()
	Script.ReloadScript("scripts/common.lua");
	Script.ReloadScript("CryMP/Scripts/Entities/Actor/BasicActor.lua");
	Script.ReloadScript("Libs/ReverbPresets/ReverbPresetDB.lua");
	-- Script.ReloadScript("Libs/SoundPresets/PresetDB.lua"); not used
	Script.ReloadScript("scripts/physics.lua");
	Script.ReloadScript("scripts/Tweaks.lua");
end

function OnShutdown()
end

function PreloadForStats()
	Script.ReloadScript("scripts/gamerules/powerstruggle.lua");

	local params={
		position={x=0, y=0, z=0},
	};

	local properties={
	};

	for i,v in pairs(PowerStruggle.buyList) do
		if (v.class) then
			params.class=v.class;
			if (v.vehicle and v.modification) then
				params.properties=properties;
				properties.Modification=v.modification;
			else
				params.properties=nil;
			end
			System.SpawnEntity(params);
			params.position.y=params.position.y+40;
			if (params.position.y>4000) then
				params.position.y=0;
				params.position.x=params.position.x+40;
			end
		end
	end
end
