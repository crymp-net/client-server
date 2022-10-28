#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/Cry3DEngine/IMaterial.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryMath/Cry_Camera.h"
#include "CrySystem/LocalizationManager.h"
#include "Library/StringTools.h"
#include "Library/Util.h"
#include "Library/WinAPI.h"

#include "ScriptBind_CPPAPI.h"
#include "Client.h"
#include "ScriptCommands.h"
#include "ScriptCallbacks.h"

ScriptBind_CPPAPI::ScriptBind_CPPAPI()
{
	Init(gEnv->pScriptSystem, gEnv->pSystem);

	SetGlobalName("CPPAPI");

#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_CPPAPI::

	SCRIPT_REG_GLOBAL(SCRIPT_CALLBACK_ON_UPDATE);
	SCRIPT_REG_GLOBAL(SCRIPT_CALLBACK_ON_DISCONNECT);
	SCRIPT_REG_GLOBAL(SCRIPT_CALLBACK_ON_SPAWN);
	SCRIPT_REG_GLOBAL(SCRIPT_CALLBACK_ON_MASTER_RESOLVED);
	SCRIPT_REG_GLOBAL(SCRIPT_CALLBACK_ON_LOADING_START);
	SCRIPT_REG_GLOBAL(SCRIPT_CALLBACK_ON_GAME_RULES_CREATED);
	SCRIPT_REG_GLOBAL(SCRIPT_CALLBACK_ON_BECOME_LOCAL_ACTOR);

	SCRIPT_REG_TEMPLFUNC(AddCCommand, "name, handler");
	SCRIPT_REG_TEMPLFUNC(ApplyMaskOne, "entity, mask, apply");
	SCRIPT_REG_TEMPLFUNC(ApplyMaskAll, "mask, apply");
	SCRIPT_REG_TEMPLFUNC(FSetCVar, "cvar, value");
	SCRIPT_REG_TEMPLFUNC(GetLocaleInformation, "");
	SCRIPT_REG_TEMPLFUNC(GetMapName, "");
	SCRIPT_REG_TEMPLFUNC(MakeUUID, "salt");
	SCRIPT_REG_TEMPLFUNC(Random, "");
	SCRIPT_REG_TEMPLFUNC(Request, "params, callback");
	SCRIPT_REG_TEMPLFUNC(SetCallback, "callback, handler");
	SCRIPT_REG_TEMPLFUNC(SHA256, "text");
	SCRIPT_REG_TEMPLFUNC(URLEncode, "text");
	SCRIPT_REG_TEMPLFUNC(GetMasters, "");
	SCRIPT_REG_FUNC(GetRenderType);
	SCRIPT_REG_TEMPLFUNC(GetKeyName, "action");
	SCRIPT_REG_TEMPLFUNC(IsKeyUsed, "key");
	SCRIPT_REG_TEMPLFUNC(CreateKeyBind, "key, command");
	SCRIPT_REG_FUNC(ClearKeyBinds);
	SCRIPT_REG_TEMPLFUNC(GetModelFilePath, "entityId, slot");
	SCRIPT_REG_TEMPLFUNC(CreateMaterialFromTexture, "materialName, texturePath");
	SCRIPT_REG_TEMPLFUNC(SetOpacity, "entityId, fAmount");

	// Localization
	SCRIPT_REG_TEMPLFUNC(GetLanguage, "");
	SCRIPT_REG_TEMPLFUNC(LocalizeText, "text");
	SCRIPT_REG_TEMPLFUNC(AddLocalizedLabel, "name, params");
}

ScriptBind_CPPAPI::~ScriptBind_CPPAPI()
{
}

int ScriptBind_CPPAPI::AddCCommand(IFunctionHandler *pH, const char *name, HSCRIPTFUNCTION handler)
{
	bool success = gClient->GetScriptCommands()->AddCommand(name, handler);

	if (!success)
		m_pSS->ReleaseFunc(handler);

	return pH->EndFunction(success);
}

int ScriptBind_CPPAPI::ApplyMaskOne(IFunctionHandler *pH, ScriptHandle entity, int mask, bool apply)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entity.n);
	if(!pEntity)
		return pH->EndFunction(false);

	IEntityRenderProxy *pRenderProxy = static_cast<IEntityRenderProxy*>(pEntity->GetProxy(ENTITY_PROXY_RENDER));
	if (!pRenderProxy)
		return pH->EndFunction(false);

	int newMask = pRenderProxy->GetMaterialLayersMask();

	if (apply)
		newMask |= mask;
	else
		newMask &= ~mask;

	pRenderProxy->SetMaterialLayersMask(newMask);

	return pH->EndFunction(true);
}

int ScriptBind_CPPAPI::ApplyMaskAll(IFunctionHandler* pH, int applyMask, bool apply) {
	IEntitySystem* pES = gEnv->pSystem->GetIEntitySystem();
	if (pES) {
		IEntityIt* it = pES->GetEntityIterator();
		int affectedEntities = 0;
		while (!it->IsEnd()) {
			IEntity* pEntity = it->This();
			int targetMask = applyMask;
			if (targetMask == MTL_LAYER_FROZEN && apply) {
				IVehicleSystem* pVS = gClient->GetGameFramework()->GetIVehicleSystem();
				if (pVS->GetVehicle(pEntity->GetId()))
					targetMask = MTL_LAYER_DYNAMICFROZEN;
			}
			IEntityRenderProxy* pRP = (IEntityRenderProxy*)pEntity->GetProxy(ENTITY_PROXY_RENDER);
			if (pRP) {
				int mask = pRP->GetMaterialLayersMask();
				if (apply)
					mask |= targetMask;
				else mask &= ~targetMask;
				pRP->SetMaterialLayersMask(mask);
			}
			affectedEntities++;
			it->Next();
		}
		return pH->EndFunction(affectedEntities);
	}
	return pH->EndFunction(0);
}

int ScriptBind_CPPAPI::FSetCVar(IFunctionHandler *pH, const char *cvar, const char *value)
{
	ICVar* pCVar = gEnv->pConsole->GetCVar(cvar);
	if (!pCVar)
	{
		return pH->EndFunction(false);
	}

	const std::string previousVal = pCVar->GetString();

	if (previousVal != std::string_view(value))
	{
		pCVar->ForceSet(value);

		// CVar still the same, is it synced?
		if (previousVal == std::string_view(pCVar->GetString()))
		{
			const int previousFlags = pCVar->GetFlags();

			// disable sync
			pCVar->SetFlags(VF_NOT_NET_SYNCED);

			// 2nd attempt
			pCVar->ForceSet(value);

			// now restore the flags
			pCVar->SetFlags(previousFlags);
			// CVar value won't change untill server changes it to something else

			if (std::string_view(value) != std::string_view(pCVar->GetString()))
			{
				CryLogAlways("$4[CryMP] Failed to change CVar %s - value still %s", cvar, pCVar->GetString());
				return pH->EndFunction(false);
			}
		}
	}

	// all good!
	return pH->EndFunction(true);
}

int ScriptBind_CPPAPI::GetLocaleInformation(IFunctionHandler *pH)
{
	return pH->EndFunction(WinAPI::GetLocale().c_str(), (int)WinAPI::GetTimeZoneBias());
}

int ScriptBind_CPPAPI::GetMapName(IFunctionHandler *pH)
{
	return pH->EndFunction(gClient->GetGameFramework()->GetLevelName());
}

int ScriptBind_CPPAPI::MakeUUID(IFunctionHandler *pH, const char *salt)
{
	return pH->EndFunction(gClient->GetHWID(salt).c_str());
}

int ScriptBind_CPPAPI::Random(IFunctionHandler *pH)
{
	return pH->EndFunction(gClient->GetRandomNumber<float>(0, 1));
}

int ScriptBind_CPPAPI::Request(IFunctionHandler *pH, SmartScriptTable params, HSCRIPTFUNCTION callback)
{
	HTTPClientRequest request;

	{
		CScriptSetGetChain chain(params);

		const char *url = "";
		const char *method = "GET";
		const char *body = "";
		SmartScriptTable headers;
		int timeout = 4000;

		chain.GetValue("url", url);
		chain.GetValue("method", method);
		chain.GetValue("body", body);
		chain.GetValue("headers", headers);
		chain.GetValue("timeout", timeout);

		request.url = url;
		request.method = method;
		request.data = body;
		request.timeout = timeout;

		if (headers)
		{
			auto it = headers->BeginIteration();
			while (headers->MoveNext(it))
			{
				if (it.sKey && it.value.GetVarType() == ScriptVarType::svtString)
				{
					request.headers[it.sKey] = it.value.str;
				}
			}
			headers->EndIteration(it);
		}
	}

	if (request.url.empty())
	{
		m_pSS->ReleaseFunc(callback);
		return pH->EndFunction(false, "url not provided");
	}

	request.callback = [callback, pSS = m_pSS](HTTPClientResult & result)
	{
		if (pSS->BeginCall(callback))
		{
			if (result.error)
				pSS->PushFuncParam(result.error.what());
			else
				pSS->PushFuncParam(false);

			pSS->PushFuncParam(result.response.c_str());
			pSS->PushFuncParam(result.code);
			pSS->EndCall();
		}

		pSS->ReleaseFunc(callback);
	};

	gClient->HttpRequest(std::move(request));

	return pH->EndFunction(true);
}

int ScriptBind_CPPAPI::SetCallback(IFunctionHandler *pH, int callback, HSCRIPTFUNCTION handler)
{
	bool success = gClient->GetScriptCallbacks()->SetHandler(static_cast<EScriptCallback>(callback), handler);

	if (!success)
		m_pSS->ReleaseFunc(handler);

	return pH->EndFunction(success);
}

int ScriptBind_CPPAPI::SHA256(IFunctionHandler *pH, const char *text)
{
	return pH->EndFunction(Util::SHA256(text).c_str());
}

int ScriptBind_CPPAPI::URLEncode(IFunctionHandler *pH, const char *text)
{
	return pH->EndFunction(HTTP::URLEncode(text).c_str());
}

int ScriptBind_CPPAPI::GetMasters(IFunctionHandler *pH)
{
	SmartScriptTable masters(m_pSS);

	for (const std::string & master : gClient->GetMasters())
	{
		masters->PushBack(master.c_str());
	}

	return pH->EndFunction(masters);
}

int ScriptBind_CPPAPI::GetRenderType(IFunctionHandler* pH)
{
	return pH->EndFunction(static_cast<int>(gEnv->pRenderer->GetRenderType()));
}

int ScriptBind_CPPAPI::GetKeyName(IFunctionHandler* pH, const char* action)
{
	IActionMapManager* pAM = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	IActionMapIteratorPtr iter = pAM->CreateActionMapIterator();
	while (IActionMap* pMap = iter->Next())
	{
		const char* actionMapName = pMap->GetName();
		IActionMapBindInfoIteratorPtr pIter = pMap->CreateBindInfoIterator();
		while (const SActionMapBindInfo* pInfo = pIter->Next())
		{
			if (!strcmp(pInfo->action, action))
				return pH->EndFunction(pInfo->keys[0]);
		}
	}

	return pH->EndFunction("");
}

int ScriptBind_CPPAPI::IsKeyUsed(IFunctionHandler* pH, const char* key)
{
	IActionMapManager* pAM = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	IActionMapIteratorPtr iter = pAM->CreateActionMapIterator();
	while (IActionMap* pMap = iter->Next())
	{
		const char* actionMapName = pMap->GetName();
		IActionMapBindInfoIteratorPtr pIter = pMap->CreateBindInfoIterator();
		while (const SActionMapBindInfo* pInfo = pIter->Next())
		{
			if (!strcmp(pInfo->keys[0], key))
				return pH->EndFunction(true);
		}
	}

	return pH->EndFunction(false);
}

int ScriptBind_CPPAPI::CreateKeyBind(IFunctionHandler* pH, const char* key, const char* action)
{
	gClient->AddKeyBind(key, action);

	return pH->EndFunction(true);
}

int ScriptBind_CPPAPI::ClearKeyBinds(IFunctionHandler* pH)
{
	gClient->ClearKeyBinds();

	return pH->EndFunction();
}

int ScriptBind_CPPAPI::GetModelFilePath(IFunctionHandler* pH, ScriptHandle entityId, int slot)
{
	IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entityId.n);
	if (!pEntity)
		return pH->EndFunction();

	if (ICharacterInstance* pCharacter = pEntity->GetCharacter(slot))
	{
		return pH->EndFunction(pCharacter->GetFilePath());
	}
	if (IStatObj* pObject = pEntity->GetStatObj(slot))
	{
		return pH->EndFunction(pObject->GetFilePath());
	}
	return pH->EndFunction();
}

int ScriptBind_CPPAPI::CreateMaterialFromTexture(IFunctionHandler* pH, const char* materialName, const char* texturePath)
{
	IMaterialManager* pMatMan(gEnv->p3DEngine->GetMaterialManager());
	IMaterial* pMat(pMatMan->FindMaterial(materialName));
	if (pMat)
	{
		CryLog("[CryMP] Material '%s' already created!", pMat->GetName());
		return pH->EndFunction();
	}

	IMaterial* pMatSrc(pMatMan->LoadMaterial("Materials/Decals/Default", false, true));
	if (pMatSrc)
	{
		IMaterial* pMatDst(pMatMan->CreateMaterial(materialName, pMatSrc->GetFlags() | MTL_FLAG_NON_REMOVABLE));
		if (pMatDst)
		{
			SShaderItem& si(pMatSrc->GetShaderItem());

			SInputShaderResources isr(si.m_pShaderResources);
			isr.m_Textures[EFTT_DIFFUSE].m_Name = texturePath;

			CryLog("[CryMP] Created Material '%s' successfully (%s)", pMatDst->GetName(), si.m_pShader->GetName());

			SShaderItem siDst(gEnv->pRenderer->EF_LoadShaderItem(si.m_pShader->GetName(), true, 0, &isr, si.m_pShader->GetGenerationMask()));
			pMatDst->AssignShaderItem(siDst);

			return pH->EndFunction();
		}
	}
	return pH->EndFunction();
}

int ScriptBind_CPPAPI::SetOpacity(IFunctionHandler* pH, ScriptHandle entity, float fAmount)
{
	IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entity.n);
	if (!pEntity)
		return pH->EndFunction(false);

	IEntityRenderProxy* pRenderProxy = static_cast<IEntityRenderProxy*>(pEntity->GetProxy(ENTITY_PROXY_RENDER));
	if (!pRenderProxy)
		return pH->EndFunction(false);

	pRenderProxy->SetOpacity(fAmount);

	return pH->EndFunction(true);
}

////////////////////////////////////////////////////////////////////////////////
// Localization
////////////////////////////////////////////////////////////////////////////////

int ScriptBind_CPPAPI::GetLanguage(IFunctionHandler* pH)
{
	const char* language = LocalizationManager::GetInstance().GetCurrentLanguage().name.c_str();

	return pH->EndFunction(language);
}

int ScriptBind_CPPAPI::LocalizeText(IFunctionHandler* pH, const char* text)
{
	const std::string result = LocalizationManager::GetInstance().Localize(text);

	return pH->EndFunction(result.c_str());
}

int ScriptBind_CPPAPI::AddLocalizedLabel(IFunctionHandler* pH, const char* name, SmartScriptTable params)
{
	LocalizationManager& localization = LocalizationManager::GetInstance();

	LocalizationManager::Label label;
	bool keepExisting = false;

	const auto readData = [&label, &keepExisting](CScriptSetGetChain& chain)
	{
		const char* value;

		if (chain.GetValue("english_text", value))
			label.englishText = value;

		if (chain.GetValue("english_subtitle", value))
			label.englishSubtitle = value;

		if (chain.GetValue("localized_text", value))
			StringTools::AssignTo(label.localizedText, value);

		if (chain.GetValue("localized_subtitle", value))
			StringTools::AssignTo(label.localizedSubtitle, value);

		if (chain.GetValue("character_name", value))
			StringTools::AssignTo(label.characterName, value);

		if (chain.GetValue("sound_event", value))
			label.soundEvent = value;

		chain.GetValue("sound_volume",           label.soundVolume);
		chain.GetValue("sound_ducking",          label.soundDucking);
		chain.GetValue("sound_radio_ratio",      label.soundRadioRatio);
		chain.GetValue("sound_radio_background", label.soundRadioBackground);
		chain.GetValue("sound_radio_squelch",    label.soundRadioSquelch);
		chain.GetValue("use_subtitle",           label.useSubtitle);

		chain.GetValue("keep_existing", keepExisting);
	};

	label.name = name;

	if (params)
	{
		CScriptSetGetChain rootChain(params);

		readData(rootChain);

		SmartScriptTable languageList;
		if (rootChain.GetValue("languages", languageList))
		{
			const char* languageName = localization.GetCurrentLanguage().name.c_str();

			SmartScriptTable language;
			if (languageList->GetValue(languageName, language))
			{
				CScriptSetGetChain languageChain(language);

				readData(languageChain);
			}
		}
	}

	const bool success = localization.Add(std::move(label), keepExisting);

	return pH->EndFunction(success);
}

////////////////////////////////////////////////////////////////////////////////
