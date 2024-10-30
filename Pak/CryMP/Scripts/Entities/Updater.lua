Updater = {

	Properties = {
		RateInMs = -1,
		Callback = nil, --function()end, --your lua function
	},

	Client = {
		---------------------------
		--       OnUpdate
		---------------------------
		OnUpdate = function(self, ft)
			if (self.Properties.Callback) then
				self.Properties.Callback(ft);
			end
		end,
	},
	---------------------------
	--       OnInit
	---------------------------
	OnInit = function(self)
	end,
	---------------------------
	--      OnSpawn
	---------------------------
	OnSpawn = function(self)
		if (tonumber(self.Properties.RateInMs) > 0) then
			self:SetScriptUpdateRate(tonumber(self.Properties.RateInMs));
		end

		self:Activate(1);
	end,
	---------------------------
	--      OnReset
	---------------------------
	OnReset = function(self)
	end,
	---------------------------
	--      SetCallback
	---------------------------
	SetCallback = function(self, callback)
		if (type(callback) ~= "function") then
			System.LogAlways("[Updater] Callback must be a function!");
			return;
		end
		self.Properties.Callback = callback;
	end,
};
