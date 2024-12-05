#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_CPPAPI : public CScriptableBase
{
public:
	ScriptBind_CPPAPI();
	~ScriptBind_CPPAPI();

	int AddCCommand(IFunctionHandler *pH, const char *name, HSCRIPTFUNCTION handler);
	int ApplyMaskOne(IFunctionHandler *pH, ScriptHandle entity, int mask, bool apply);
	int ApplyMaskAll(IFunctionHandler* pH, int mask, bool apply);
	int FSetCVar(IFunctionHandler *pH, const char *cvar, const char *value);
	int GetLocaleInformation(IFunctionHandler *pH);
	int GetMapName(IFunctionHandler *pH);
	int MakeUUID(IFunctionHandler *pH, const char *salt);
	int Random(IFunctionHandler *pH);
	int Request(IFunctionHandler *pH, SmartScriptTable params, HSCRIPTFUNCTION callback);
	int SetCallback(IFunctionHandler *pH, int callback, HSCRIPTFUNCTION handler);
	int SHA256(IFunctionHandler *pH, const char *text);
	int URLEncode(IFunctionHandler *pH, const char *text);
	int GetMasters(IFunctionHandler *pH);
	int GetRenderType(IFunctionHandler* pH);
	int GetKeyName(IFunctionHandler* pH, const char* action);
	int IsKeyUsed(IFunctionHandler* pH, const char* key);
	int CreateKeyBind(IFunctionHandler* pH, const char* key, const char* action);
	int CreateKeyFunction(IFunctionHandler* pH, const char* key, HSCRIPTFUNCTION function);
	int ClearKeyBinds(IFunctionHandler* pH);
	int GetModelFilePath(IFunctionHandler* pH, ScriptHandle entityId, int slot);
	int CreateMaterialFromTexture(IFunctionHandler* pH, const char* materialName, const char* texturePath);
	int SetOpacity(IFunctionHandler* pH, ScriptHandle entity, float fAmount);
	int GetLastSeenTime(IFunctionHandler* pH, ScriptHandle entity);
	int GetLP(IFunctionHandler* pH);
	int GetNumVars(IFunctionHandler* pH);
	int GetVars(IFunctionHandler* pH);

	////////////////////////////////////////////////////////////////////////////////
	// Localization
	////////////////////////////////////////////////////////////////////////////////
	int GetLanguage(IFunctionHandler* pH);
	int LocalizeText(IFunctionHandler* pH, const char* text);
	int AddLocalizedLabel(IFunctionHandler* pH, const char* name, SmartScriptTable params);

	////////////////////////////////////////////////////////////////////////////////
	// DrawTools
	////////////////////////////////////////////////////////////////////////////////
	int DrawText(IFunctionHandler* pH, float posX, float posY, float xscale, float yscale, float color1, float color2, float color3, float color4, const char* text);
	int DrawImage(IFunctionHandler* pH, float posX, float posY, float width, float height, const char* texturePath);
	int DrawColorBox(IFunctionHandler* pH, float posX, float posY, float width, float height, float color1, float color2, float color3, float opacity);
	int RemoveTextOrImageById(IFunctionHandler* pH, int id);
	int RemoveTextOrImageAll(IFunctionHandler* pH);
	
	int GetLoadingScreenMapPicturePath(IFunctionHandler* pH, const char* level);
	int FOVEffect(IFunctionHandler* pH, float goalFOV, float speed);

	int SetAttachmentMaterial(IFunctionHandler* pH, ScriptHandle entityId, int characterSlot, const char* attachmentName, const char* materialName);
	int GetCharacterAttachments(IFunctionHandler* pH, ScriptHandle entityId, int characterSlot);
	int GetCharacterJoints(IFunctionHandler* pH, ScriptHandle entityId, int characterSlot);
	int CreateCharacterDecal(IFunctionHandler* pH, ScriptHandle entityId, int characterSlot, SmartScriptTable params);

	////////////////////////////////////////////////////////////////////////////////
	// ActionFilter
	////////////////////////////////////////////////////////////////////////////////
	int EnableActionFilter(IFunctionHandler* pH, const char* name, bool enable);
	int IsActionFilterEnabled(IFunctionHandler* pH, const char* name);
	int IsActionFilterAvailable(IFunctionHandler* pH, const char* name);
	int CreateActionFilter(IFunctionHandler* pH, const char* name, SmartScriptTable keys);
};
