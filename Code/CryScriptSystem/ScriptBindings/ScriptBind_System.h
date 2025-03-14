#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_System : public CScriptableBase
{
	SmartScriptTable m_pScriptTimeTable;

public:
	ScriptBind_System(IScriptSystem *pSS);

	int LoadFont(IFunctionHandler *pH);
	int ExecuteCommand(IFunctionHandler *pH);
	int LogToConsole(IFunctionHandler *pH);
	int ClearConsole(IFunctionHandler *pH);
	int Log(IFunctionHandler *pH);
	int LogAlways(IFunctionHandler *pH);
	int Warning(IFunctionHandler *pH);
	int Error(IFunctionHandler *pH);
	int IsEditor(IFunctionHandler *pH);
	int IsEditing(IFunctionHandler *pH);
	int GetCurrTime(IFunctionHandler *pH);
	int GetCurrAsyncTime(IFunctionHandler *pH);
	int GetFrameTime(IFunctionHandler *pH);
	int GetLocalOSTime(IFunctionHandler *pH);
	int GetUserName(IFunctionHandler *pH);
	int ShowConsole(IFunctionHandler *pH);
	int IsMultiplayer(IFunctionHandler *pH);
	int GetEntity(IFunctionHandler *pH);
	int GetEntityClass(IFunctionHandler *pH);
	int GetEntities(IFunctionHandler *pH);
	int GetEntitiesByClass(IFunctionHandler *pH, const char *EntityClass);
	int GetEntitiesInSphere(IFunctionHandler *pH, Vec3 center, float radius);
	int GetEntitiesInSphereByClass(IFunctionHandler *pH, Vec3 center, float radius, const char *EntityClass);
	int GetPhysicalEntitiesInBox(IFunctionHandler *pH, Vec3 center, float radius);
	int GetPhysicalEntitiesInBoxByClass(IFunctionHandler *pH, Vec3 center, float radius, const char *className);
	int GetNearestEntityByClass(IFunctionHandler *pH, Vec3 center, float radius, const char *className);
	int GetEntityByName(IFunctionHandler *pH, const char *sEntityName);
	int GetEntityIdByName(IFunctionHandler *pH, const char *sEntityName);
	int DrawLabel(IFunctionHandler *pH);
	int DeformTerrain(IFunctionHandler *pH);
	int DeformTerrainUsingMat(IFunctionHandler *pH);
	int ApplyForceToEnvironment(IFunctionHandler *pH);
	int ScreenToTexture(IFunctionHandler *pH);
	int DrawLine(IFunctionHandler *pH);
	int Draw2DLine(IFunctionHandler *pH);
	int DrawText(IFunctionHandler *pH);
	int DrawSphere(IFunctionHandler *pH, float x, float y, float z, float radius, int r, int g, int b, int a);
	int DrawAABB(IFunctionHandler *pH, float x, float y, float z, float x2, float y2, float z2, int r, int g, int b, int a);
	int DrawOBB(IFunctionHandler *pH, float x, float y, float z, float w, float h, float d, float rx, float ry, float rz);
	int SetGammaDelta(IFunctionHandler *pH);
	int SetPostProcessFxParam(IFunctionHandler *pH);
	int GetPostProcessFxParam(IFunctionHandler *pH);
	int SetScreenFx(IFunctionHandler *pH);
	int GetScreenFx(IFunctionHandler *pH);
	int SetCVar(IFunctionHandler *pH);
	int GetCVar(IFunctionHandler *pH);
	int AddCCommand(IFunctionHandler *pH);
	int SetScissor(IFunctionHandler *pH);
	int IsHDRSupported(IFunctionHandler *pH);
	int SetWind(IFunctionHandler *pH);
	int GetWind(IFunctionHandler *pH);
	int GetSurfaceTypeIdByName(IFunctionHandler *pH, const char *surfaceName);
	int GetSurfaceTypeNameById(IFunctionHandler *pH, int surfaceId);
	int RemoveEntity(IFunctionHandler *pH, ScriptHandle entityId);
	int SpawnEntity(IFunctionHandler *pH, SmartScriptTable params);
	int IsValidMapPos(IFunctionHandler *pH);
	int EnableOceanRendering(IFunctionHandler *pH);
	int ScanDirectory(IFunctionHandler *pH);
	int ViewDistanceGet(IFunctionHandler *pH);
	int GetOutdoorAmbientColor(IFunctionHandler *pH);
	int GetTerrainElevation(IFunctionHandler *pH);
	int ActivatePortal(IFunctionHandler *pH);
	int IsPointIndoors(IFunctionHandler *pH);
	int ProjectToScreen(IFunctionHandler *pH, Vec3 vec);
	int Break(IFunctionHandler *pH);
	int SetViewCameraFov(IFunctionHandler *pH, float fov);
	int GetViewCameraFov(IFunctionHandler *pH);
	int IsPointVisible(IFunctionHandler *pH, Vec3 point);
	int GetViewCameraPos(IFunctionHandler *pH);
	int GetViewCameraDir(IFunctionHandler *pH);
	int GetViewCameraUpDir(IFunctionHandler *pH);
	int GetViewCameraAngles(IFunctionHandler *pH);
	int IsDevModeEnable(IFunctionHandler *pH);
	int SaveConfiguration(IFunctionHandler *pH);
	int Quit(IFunctionHandler *pH);
	int GetFrameID(IFunctionHandler *pH);
	int ClearKeyState(IFunctionHandler *pH);
	int SetSunColor(IFunctionHandler *pH, Vec3 vColor);
	int GetSunColor(IFunctionHandler *pH);
	int SetSkyColor(IFunctionHandler *pH, Vec3 vColor);
	int GetSkyColor(IFunctionHandler *pH);
	int SetSkyHighlight(IFunctionHandler *pH, SmartScriptTable params);
	int GetSkyHighlight(IFunctionHandler *pH, SmartScriptTable params);
	int LoadLocalizationXml(IFunctionHandler *pH, const char *filename);
	int GetActors(IFunctionHandler* pH);
	int GetActorsByClass(IFunctionHandler* pH, const char* entityClass);
	int GetPlayers(IFunctionHandler* pH);
	int GetVehicles(IFunctionHandler* pH);
	int GetVehiclesByClass(IFunctionHandler* pH, const char* entityClass);
	int IsClassValid(IFunctionHandler* pH, const char* entityClass);
	int GetEntityScriptFilePath(IFunctionHandler* pH, const char* entityClass);

	int GetVehicleClasses(IFunctionHandler* pH);
	int AwakeDefaultObjects(IFunctionHandler* pH);
	int GetItemClasses(IFunctionHandler* pH);
	int GetCountOfClass(IFunctionHandler* pH, const char* EntityClass);
	int GetEntityIdByNumId(IFunctionHandler* pH, int entityId);
	int RemoveEntitiesByClass(IFunctionHandler* pH, const char* EntityClass);
};
