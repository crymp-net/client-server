----------------------------------------------------
-- Common Globals and Definitions.
----------------------------------------------------
----------------------------------------------------

-- data structure passed to the Signals, use this global to avoid temporary lua mem allocation
g_SignalData_point = {x=0,y=0,z=0};
g_SignalData_point2 = {x=0,y=0,z=0};

-- REMEMBER! ALWAYS write 
-- g_SignalData.point = g_SignalData_point 
-- before doing direct value assignment (i.e. not referenced) like
-- g_SignalData.point.x = ...
-- and any math.lua vector function on it (FastSumVectors(g_SignalData.point,..) etc

g_SignalData = {
	point = g_SignalData_point, -- since g_SignalData.point is always used as a handler
	point2 = g_SignalData_point2,
	ObjectName = "",
	id = NULL_ENTITY,
	fValue = 0,
	iValue = 0,
	iValue2 = 0,
}

g_StringTemp1 = "                                            ";

g_HitTable = {{},{},{},{},{},{},{},{},{},{},}


function ShowTime()
	local ttime=System.GetLocalOSTime();	
	System.Log(string.format("%d/%d/%d, %02d:%02d", ttime.mday, ttime.mon+1, ttime.year+1900, ttime.hour, ttime.min));
end

function count(_tbl)
	local count = 0;
	if (_tbl) then
		for i,v in pairs(_tbl) do
			count = count+1;
		end
	end	
	return count;
end


function new(_obj, norecurse)
	if (type(_obj) == "table") then
		local _newobj = {};
		if (norecurse) then
			for i,f in pairs(_obj) do
				_newobj[i] = f;
			end
		else
			for i,f in pairs(_obj) do
				if ((type(f) == "table") and (_obj~=f)) then -- avoid recursing into itself
					_newobj[i] = new(f);
				else
					_newobj[i] = f;
				end
			end
		end
		return _newobj;
	else
		return _obj;
	end
end


function merge(dst, src, recurse)
	for i,v in pairs(src) do
		if (type(v) ~= "function") then
			if(recurse) then
				if((type(v) == "table") and (v ~= src))then  -- avoid recursing into itself
					if (not dst[i]) then
						dst[i] = {};
					end
					merge(dst[i], v, recurse);
				elseif (not dst[i]) then
					dst[i] = v;
				end
			elseif (not dst[i]) then
				dst[i] = v;
			end
		end
	end
	
	return dst;
end


function mergef(dst, src, recursive)
	for i,v in pairs(src) do
		if (recursive) then
			if((type(v) == "table") and (v ~= src))then  -- avoid recursing into itself
				if (not dst[i]) then
					dst[i] = {};
				end
				mergef(dst[i], v, recursive);
			elseif (not dst[i]) then
				dst[i] = v;
			end
		elseif (not dst[i]) then
			dst[i] = v;
		end
	end
	
	return dst;
end


function Vec2Str(vec)
	return string.format("(x: %.3f y: %.3f z: %.3f)", vec.x, vec.y, vec.z);
end


function LogError(fmt, ...)
	System.Log("$4"..string.format(fmt, ...));
end


function LogWarning(fmt, ...)
	System.Log("$6"..string.format(fmt, ...));
end


function Log(fmt, ...)
	System.Log(string.format(fmt, ...));
end


g_dump_tabs=0;
function dump(_class, no_func, depth)
	local log = System.LogAlways;
	if not _class then
		log("$2nil");
	else
		if (not depth) then
			depth = 8;
		end
		local str="";
		for n=0,g_dump_tabs,1 do
			str=str.."  ";
		end
		for i,field in pairs(_class) do
			if(type(field)=="table") then
				if (g_dump_tabs < depth) then
					g_dump_tabs=g_dump_tabs+1;
					log(str.."$4"..tostring(i).."$1= {");
					dump(field, no_func, depth);
					log(str.."$1}");
					g_dump_tabs=g_dump_tabs-1;
				else
					log(str.."$4"..tostring(i).."$1= { $4...$1 }");
				end
			else
				if(type(field)=="number" ) then
					log("$2"..str.."$6"..tostring(i).."$1=$8"..field);
				elseif(type(field) == "string") then
					log("$2"..str.."$6"..tostring(i).."$1=$8".."\""..field.."\"");
				elseif(type(field) == "boolean") then
					log("$2"..str.."$6"..tostring(i).."$1=$8".."\""..tostring(field).."\"");
				else
					if(not no_func)then
						if(type(field)=="function")then
							log("$2"..str.."$5"..tostring(i).."()");
						else
							log("$2"..str.."$7"..tostring(i).."$8<userdata>");
						end
					end
				end
			end
		end
	end
end


-- check if a string is set and it's length > 0
function EmptyString(str)
	if (str and string.len(str) > 0) then
		return false;
	end
	return true;
end

-- check if a number value is true or false
-- usefull for entity parameters
function NumberToBool(n)
	if (n and (tonumber(n) ~= 0)) then
		return true;
	else
		return false;
	end
end



-- easy way to log entity id
-- accepts both entity table or entityid
function EntityName(entity)
	if (type(entity) == "userdata") then
		local e = System.GetEntity(entity);
		if (e) then
			return e:GetName();
		end
	elseif (type(entity) == "table") then
		return entity:GetName();
	end
	return "";
end


-- easy way to get entity by name
-- usefull for "console debugging"!
function EntityNamed(name)
	return System.GetEntityByName(name);
end

function SafeTableGet( table, name )
	if table then return table[name] else return nil end
end

----------------------------------------------------
-- Load commonly used globals.
----------------------------------------------------
Script.ReloadScript("scripts/Utils/Math.lua");
Script.ReloadScript("scripts/Utils/EntityUtils.lua");
Script.ReloadScript("scripts/Utils/ZeroG.lua");
----------------------------------------------------

--///////////////////////////////////////////////////////////////////////////////////
-- needed to reload ai related scripts from editor
----------------------------------------------------
function AIReload()
	Script.LoadScript("Scripts/AI/aiconfig.lua",true,true);
end


g_AIDebugToggleOn=0;
--///////////////////////////////////////////////////////////////////////////////////
-- 
----------------------------------------------------
function AIDebugToggle()
	if(g_AIDebugToggleOn == 0) then
--		System.Log("___AIDebugToggle switching on");
		g_AIDebugToggleOn=1;
		System.SetCVar( "ai_DebugDraw",1 );
	else
--		System.Log("___AIDebugToggle switching off");
		System.SetCVar( "ai_DebugDraw",0 );
		g_AIDebugToggleOn=0;	
	end
end




----------------------------------------------------

Script.ReloadScript("scripts/entities/items/itemsystemmath.lua");

--///////////////////////////////////////////////////////////////////////////////////
-- VS2 hack until we find a better place to do that
-- precache bullet_hits sound so the wavebank is loaded statically
Sound.Precache("Sounds/physics:bullet_impact:mat_grass", SOUND_PRECACHE_LOAD_SOUND);   -- bullet_hit fsb
Sound.Precache("Sounds/physics:footstep_walk:mat_grass", SOUND_PRECACHE_LOAD_SOUND);   -- footsteps fsb
Sound.Precache("Sounds/physics:mat_metal_sheet:mat_dirt", SOUND_PRECACHE_LOAD_SOUND); -- collision fsb
