Script.ReloadScript("Scripts/Entities/AI/Shared/BasicAI.lua");
Script.ReloadScript("CryMP/Scripts/Entities/AI/Aliens/Alien_x.lua");

CreateAlien(Alien_x);
Alien=CreateAI(Alien_x);
Alien:Expose();
