Script.ReloadScript("CryMP/Scripts/Entities/AI/Grunt_x.lua");
-----------------------------------------------------------------------------------------------------

CreateActor(Grunt_x);
Grunt = CreateAI(Grunt_x);
Grunt:Expose();
--MakeSpawnable(Grunt)
