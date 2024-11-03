--------------------------------------------------------------------------
--	Crytek Source File.
-- 	Copyright (C), Crytek Studios, 2001-2004.
--------------------------------------------------------------------------
--	$Id$
--	$DateTime$
--	Description: Common code for all the vehicle implementations
--  
--------------------------------------------------------------------------
--  History:
--  - 17/12/2004   14:43 : Created by Mathieu Pinard
--
--------------------------------------------------------------------------

V_XML_DEF = "Scripts/Entities/Vehicles/def_vehicle.xml";
V_SCRIPT_DIR = "Scripts/Entities/Vehicles/Implementations/";
V_XML_DIR = V_SCRIPT_DIR.."Xml/";

-- convenience defines to use with GetHelperPos, GetHelperDir (on vehicle and entity)
HELPER_WORLD = 0;
HELPER_LOCAL = 1;


VehicleSystem = {};


-- get vehicle implementations
if (not VehicleSystem.VehicleImpls) then  
  VehicleSystem.VehicleImpls = Vehicle.GetVehicleImplementations(V_XML_DIR);
  Log("[VehicleSystem]: got %i vehicle implementations", count(VehicleSystem.VehicleImpls));
end


--------------------------------------------------------------------------
function ExposeVehicleToNetwork( class )
	Net.Expose {
		Class = class,
		ClientMethods = {
		},

		ServerMethods = {
		},
	}
end


--------------------------------------------------------------------------
function VehicleSystem.LoadXML(vehicleImpl)
  local dataFile = V_XML_DIR..vehicleImpl..".xml";  
  local dataTable = CryAction.LoadXML( V_XML_DEF, dataFile );
  --Log("LoadXML: "..dataFile);      
  return dataTable;
end

--------------------------------------------------------------------------
function VehicleSystem.ReloadVehicleSystem()
  Log("Reloading VehicleSystem");
  Vehicle.ReloadSystem();
  Script.ReloadScript( "Scripts/Entities/Vehicles/VehicleSystem.lua" );        
end


--------------------------------------------------------------------------
function ForcePassengersToExit(vehicleName)
	local v = System.GetEntityByName(vehicleName);
	
	if (v and v.vehicle) then
		for i,seat in pairs(v.Seats) do
			local passengerId = seat:GetPassengerId();
			if (passengerId) then
				v:LeaveVehicle(passengerId);
			end
		end
	end
end

--------------------------------------------------------------------------
function DestroyVehicle(vehicleName)
	local v = System.GetEntityByName(vehicleName);	
	
	if (v and v.vehicle) then
		v.allClients:Destroy(true);
	end
end

--------------------------------------------------------------------------
function LogParent(entityName)
	local e = System.GetEntityByName(entityName);
	local p = e:GetParent();
	e:GetWorldPos();
	
	if (p) then
		Log("Entity %s does have a parent %s.", entityName, p:GetName());
	else
		Log("Entity %s does not have any parent.", entityName);
	end
end

--------------------------------------------------------------------------
function VehicleSystem.SetTpvDistance(dist)
  Vehicle.SetTpvDistance(tonumber(dist));
end

--------------------------------------------------------------------------
function VehicleSystem.SetTpvHeight(height)
  Vehicle.SetTpvHeight(tonumber(height));
end