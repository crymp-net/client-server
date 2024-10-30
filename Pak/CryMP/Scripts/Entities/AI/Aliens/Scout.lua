Script.ReloadScript("CryMP/Scripts/Entities/AI/Aliens/Scout_x.lua");

CreateAlien(Scout_x);
Scout = CreateAI(Scout_x)
Scout:Expose();


function scouttest()
	local a = System.GetEntityByName("Scout1");

	a:GrabEntity("DongFeng1");
end

function scouttest2()
	local a = System.GetEntityByName("Scout1");

	a:DropGrab();
end
