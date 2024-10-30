Script.ReloadScript("CryMP/Scripts/Entities/AI/Civilian_x.lua");

CreateActor(Civilian_x);
Civilian=CreateAI(Civilian_x)

-----------------------------------------------------------------------------------------------------
function Civilian.Client:OnTimer(timerId,mSec)
	if(timerId == HOSTAGE_COWER_TIMER ) then
		self.AI.Cower = false;
		AI.Signal(SIGNALFILTER_SENDER,1,"END_COWER",self.id);
	else
		BasicActor.Client.OnTimer(self,timerId,mSec);
	end
end


Civilian:Expose();
