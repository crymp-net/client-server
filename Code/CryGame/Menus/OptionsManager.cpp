/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Handles options setting, getting, saving and loading.

-------------------------------------------------------------------------
History:
- 03/2007: Created by Jan Müller

*************************************************************************/

#include <cstring>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/CryFile.h"
#include "OptionsManager.h"
#include "CryCommon/CryAction/IPlayerProfiles.h"
#include "FlashMenuObject.h"
#include "FlashMenuScreen.h"
#include "CryGame/Game.h"
#include "CryGame/GameCVars.h"
#include "CryGame/HUD/HUD.h"
#include "CrySystem/GameWindow.h"

//-----------------------------------------------------------------------------------------------------

#define CRYSIS_PROFILE_COLOR_AMBER "12612932"
#define CRYSIS_PROFILE_COLOR_BLUE  "5079987"
#define CRYSIS_PROFILE_COLOR_GREEN "4481854"
#define CRYSIS_PROFILE_COLOR_RED   "7474188"
#define CRYSIS_PROFILE_COLOR_WHITE "13553087"

//-----------------------------------------------------------------------------------------------------

COptionsManager* COptionsManager::sp_optionsManager = NULL;

//-----------------------------------------------------------------------------------------------------

COptionsManager::COptionsManager() : m_pPlayerProfileManager(NULL)
{
	m_defaultColorLine = "4481854";
	m_defaultColorOver = "14125840";
	m_defaultColorText = "12386209";

	m_eCrysisProfileColor = CrysisProfileColor_Default;

	m_pbEnabled = false;
	m_firstStart = false;
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SetCrysisProfileColor(const char* color)
{
	if (!color)
	{
		m_eCrysisProfileColor = CrysisProfileColor_Default;
	}
	else if (std::strcmp(color, CRYSIS_PROFILE_COLOR_AMBER) == 0)
	{
		m_eCrysisProfileColor = CrysisProfileColor_Amber;
	}
	else if (std::strcmp(color, CRYSIS_PROFILE_COLOR_BLUE) == 0)
	{
		m_eCrysisProfileColor = CrysisProfileColor_Blue;
	}
	else if (std::strcmp(color, CRYSIS_PROFILE_COLOR_GREEN) == 0)
	{
		m_eCrysisProfileColor = CrysisProfileColor_Green;
	}
	else if (std::strcmp(color, CRYSIS_PROFILE_COLOR_RED) == 0)
	{
		m_eCrysisProfileColor = CrysisProfileColor_Red;
	}
	else if (std::strcmp(color, CRYSIS_PROFILE_COLOR_WHITE) == 0)
	{
		m_eCrysisProfileColor = CrysisProfileColor_White;
	}

	GameWindow::GetInstance().UpdateCursorColor();
}

//-----------------------------------------------------------------------------------------------------

ECrysisProfileColor COptionsManager::GetCrysisProfileColor()
{
	return m_eCrysisProfileColor;
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SetProfileManager(IPlayerProfileManager* pProfileMgr)
{
	if (gEnv->pSystem->IsEditor() || gEnv->pSystem->IsDedicated())
		return;

	m_pPlayerProfileManager = pProfileMgr;
	//don't do that, as there is no profile yet.
	//InitProfileOptions();
	//if (g_pGame->GetOptions())
	//	g_pGame->GetOptions()->UpdateFlashOptions();

}
//-----------------------------------------------------------------------------------------------------

const char* COptionsManager::GetProfileName()
{
	if (!m_pPlayerProfileManager)
		return "Nomad";
	const char* user = m_pPlayerProfileManager->GetCurrentUser();
	if (!user)
		return "Nomad";
	IPlayerProfile* profile = m_pPlayerProfileManager->GetCurrentProfile(user);
	if (!profile)
		return "Nomad";
	if (!_stricmp(profile->GetName(), "default"))
		return "Nomad";
	return profile->GetName();

}

//-----------------------------------------------------------------------------------------------------

bool COptionsManager::IgnoreProfile()
{
	return GetISystem()->IsDevMode() && (g_pGameCVars->g_useProfile == 0);
}


static bool IsOption(const char* attribName, const char*& cVarName, bool& bWriteToCfg)
{
	const char optionPrefix[] = "Option.";
	const char optionPrefixConfig[] = "OptionCfg.";
	const size_t optionPrefixLen = sizeof(optionPrefix) - 1;
	const size_t optionPrefixConfigLen = sizeof(optionPrefixConfig) - 1;
	if (strncmp(attribName, optionPrefix, optionPrefixLen) == 0)
	{
		cVarName = attribName + optionPrefixLen;
		bWriteToCfg = false;
		return true;
	}
	if (strncmp(attribName, optionPrefixConfig, optionPrefixConfigLen) == 0)
	{
		cVarName = attribName + optionPrefixConfigLen;
		bWriteToCfg = true;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::InitProfileOptions(bool switchProfiles)
{
	if (!m_pPlayerProfileManager)
		return;

	if (g_pGameCVars->g_useProfile == 0) return;

	if (g_pGameCVars->g_startFirstTime == 1)
	{
		ICVar* pCVar = gEnv->pConsole->GetCVar("g_startFirstTime");
		if (pCVar && pCVar->GetIVal() == 1)
		{
			pCVar->Set(0);
			m_firstStart = true;
		}
		switchProfiles = true;
	}

	const char* user = m_pPlayerProfileManager->GetCurrentUser();
	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(user);
	if (!pProfile)
		return;

	IAttributeEnumeratorPtr attribEnum = pProfile->CreateAttributeEnumerator();
	IAttributeEnumerator::SAttributeDescription attrib;

	m_profileOptions.clear();
	while (attribEnum->Next(attrib))
	{
		bool bWriteToCfg = false;
		const char* attribCVar = "";
		const bool bIsOption = IsOption(attrib.name, attribCVar, bWriteToCfg);

		if (bIsOption)
		{
			SOptionEntry& entry = m_profileOptions[attribCVar];
			entry.name = attrib.name;
			entry.writeToConfig = bWriteToCfg;

			if (!bWriteToCfg || switchProfiles)
			{
				string value;
				if (!strcmp(attribCVar, "pb_client"))
				{
					GetProfileValue(attrib.name, value);
					if (atoi(value.c_str()) == 0)
					{
						m_pbEnabled = false;
						gEnv->pConsole->ExecuteString("net_pb_cl_enable false");
					}
					else
					{
						m_pbEnabled = true;
						gEnv->pConsole->ExecuteString("net_pb_cl_enable true");
					}
					continue;
				}
				else if (!strcmp(attribCVar, "fsaa_mode"))
				{
					GetProfileValue(attrib.name, value);
					SetAntiAliasingMode(value.c_str());
				}
				else if (!strcmp(attribCVar, "g_difficultyLevel"))
				{
					GetProfileValue(attrib.name, value);
					SetDifficulty(value.c_str());
				}

				ICVar* pCVar = gEnv->pConsole->GetCVar(attribCVar);
				if (pCVar && GetProfileValue(attrib.name, value))
				{
					if (_stricmp(pCVar->GetString(), value.c_str()))
					{
						//CryLogAlways("Inited, loaded and changed: %s = %s (was %s)", attrib.name, value, pCVar->GetString());
						pCVar->Set(value.c_str());
					}
					else
					{
						//CryLogAlways("Inited, loaded, but not changed: %s = %s", attrib.name, value);
					}
					if (!_stricmp(attrib.name, "Option.hud_colorLine"))
					{
						SetCrysisProfileColor(value.c_str());
					}
				}
			}
		}
	}

	WriteGameCfg();

}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::ResetDefaults(const char* option)
{
	if (!m_pPlayerProfileManager)
		return;

	const char* user = m_pPlayerProfileManager->GetCurrentUser();
	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(user);
	if (!pProfile)
		return;
	XmlNodeRef root = GetISystem()->LoadXmlFile("libs/config/profiles/default/attributes.xml");
	bool resetAll = (option == NULL);
	bool detectHardware = false;
	for (int i = 0; i < root->getChildCount(); ++i)
	{
		XmlNodeRef enumNameNode = root->getChild(i);
		const char* name = enumNameNode->getAttr("name");
		const char* value = enumNameNode->getAttr("value");
		if (name && value)
		{
			const char* attribCVar = "";
			bool bWriteToCfg = false;
			const bool bIsOption = IsOption(name, attribCVar, bWriteToCfg);
			if (bIsOption)
			{
				if (!resetAll && strcmp(attribCVar, option))
					continue;

				if (!strcmp(attribCVar, "sys_spec_Shadows"))
				{
					detectHardware = true;
				}

				if (!strcmp(attribCVar, "hud_colorLine"))
				{
					CryFixedStringT<32> color;
					color.Format("%d", g_pGameCVars->hud_colorLine);
					SetCrysisProfileColor(color.c_str());
				}

				if (!strcmp(attribCVar, "pb_client"))
				{
					if (atoi(value) == 0)
					{
						m_pbEnabled = false;
						gEnv->pConsole->ExecuteString("net_pb_cl_enable false");
					}
					else
					{
						m_pbEnabled = true;
						gEnv->pConsole->ExecuteString("net_pb_cl_enable true");
					}
				}
				else if (!strcmp(attribCVar, "g_difficultyLevel"))
				{
					SetDifficulty(value);
				}
				else
				{
					ICVar* pCVar = gEnv->pConsole->GetCVar(attribCVar);
					if (pCVar)
					{
						pCVar->Set(value);
					}
				}
				if (!resetAll)
					break;
			}
		}
	}
	if (detectHardware)
		AutoDetectHardware("");
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::UpdateFlashOptions()
{
	CFlashMenuScreen* pMainMenu = SAFE_MENU_FUNC_RET(GetMenuScreen(CFlashMenuObject::MENUSCREEN_FRONTENDSTART));
	CFlashMenuScreen* pInGameMenu = SAFE_MENU_FUNC_RET(GetMenuScreen(CFlashMenuObject::MENUSCREEN_FRONTENDINGAME));

	CFlashMenuScreen* pCurrentMenu = NULL;
	if (pMainMenu && pMainMenu->IsLoaded())
		pCurrentMenu = pMainMenu;
	else if (pInGameMenu && pInGameMenu->IsLoaded())
		pCurrentMenu = pInGameMenu;

	if (!pCurrentMenu) return;

	for (const auto& [cvar, entry] : m_profileOptions)
	{
		if (cvar == "pb_client")
		{
			SFlashVarValue option[3] = { "pb_client", m_pbEnabled, true };
			pCurrentMenu->Invoke("Root.MainMenu.Options.SetOption", option, 3);
		}
		else if (cvar == "fsaa_mode")
		{
			if (g_pGame->GetMenu())
			{
				if (!gEnv->pConsole->GetCVar("r_FSAA")->GetIVal())
				{
					SFlashVarValue option[3] = { "fsaa_mode", "0", true };
					pCurrentMenu->Invoke("Root.MainMenu.Options.SetOption", option, 3);
				}
				else
				{
					int samples = gEnv->pConsole->GetCVar("r_FSAA_samples")->GetIVal();
					int quality = gEnv->pConsole->GetCVar("r_FSAA_quality")->GetIVal();
					SFlashVarValue option[3] = { "fsaa_mode", g_pGame->GetMenu()->GetFSAAMode(samples, quality).c_str(), true };
					pCurrentMenu->Invoke("Root.MainMenu.Options.SetOption", option, 3);
				}
			}
		}
		else
		{
			ICVar* pCVar = gEnv->pConsole->GetCVar(cvar.c_str());
			if (pCVar)
			{
				const char* name = pCVar->GetName();
				const char* value = pCVar->GetString();

				bool bIsValid = pCVar->GetIVal() == pCVar->GetRealIVal();

				if (!_stricmp(name, "r_fsaa_samples")) //fsaa workaround for RnD
				{
					ICVar* pFSAA = gEnv->pConsole->GetCVar("r_fsaa");
					if (pFSAA && pFSAA->GetIVal() == 0)
						value = pFSAA->GetString();
				}

				SFlashVarValue option[3] = { name, value, bIsValid };
				pCurrentMenu->Invoke("Root.MainMenu.Options.SetOption", option, 3);
			}
		}

	}
	pCurrentMenu->CheckedInvoke("_root.Root.MainMenu.Options.updateOptions");
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::UpdateToProfile()
{
	for (const auto& [cvar, entry] : m_profileOptions)
	{
		if (cvar == "pb_client")
		{
			SaveValueToProfile(entry.name.c_str(), m_pbEnabled ? 1 : 0);
		}
		else if (cvar == "fsaa_mode")
		{
			string value("");
			if (g_pGame->GetMenu())
			{
				if (gEnv->pConsole->GetCVar("r_FSAA")->GetIVal())
				{
					int samples = gEnv->pConsole->GetCVar("r_FSAA_samples")->GetIVal();
					int quality = gEnv->pConsole->GetCVar("r_FSAA_quality")->GetIVal();
					value = g_pGame->GetMenu()->GetFSAAMode(samples, quality);
				}
			}
			SaveValueToProfile(entry.name.c_str(), value);
		}
		else
		{
			ICVar* pCVar = gEnv->pConsole->GetCVar(cvar.c_str());
			if (pCVar)
			{
				string value(pCVar->GetString());
				SaveValueToProfile(entry.name.c_str(), value);
			}
		}
	}
	SaveProfile();
}

//-----------------------------------------------------------------------------------------------------
bool COptionsManager::HandleFSCommand(const char* szCommand, const char* szArgs)
{

	if (!m_pPlayerProfileManager)
		return false;

	const char* user = m_pPlayerProfileManager->GetCurrentUser();
	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(user);
	if (!pProfile)
		return false;

	if (!_stricmp(szCommand, "SaveProfile"))
	{
		UpdateToProfile();
		SaveProfile();
		return true;
	}

	if (!_stricmp(szCommand, "RestoreDefaultProfile"))
	{
		if (szArgs && szArgs[0])
			ResetDefaults(szArgs);
		else
			ResetDefaults(NULL);
		return true;
	}

	if (!_stricmp(szCommand, "UpdateCVars"))
	{
		UpdateFlashOptions();
		return true;
	}

	if (!_stricmp(szCommand, "hud_showAllObjectives"))
	{
		if (szArgs)
		{
			SAFE_HUD_FUNC(SetShowAllOnScreenObjectives(atoi(szArgs) ? true : false));
		}
	}
	else if (!_stricmp(szCommand, "hud_colorLine"))
	{
		SetCrysisProfileColor(szArgs);
	}
	else if (gEnv->bMultiplayer && !_stricmp(szCommand, "g_psTutorial_Enabled"))
	{
		if (atoi(szArgs) == 1)
		{
			gEnv->pConsole->ExecuteString("g_psTutorial_Reset");
		}
	}

	if (m_profileOptions.contains(szCommand))
	{
		ICVar* pCVar = gEnv->pConsole->GetCVar(szCommand);
		if (pCVar)
		{
			pCVar->Set(szArgs);
			return true;
		}
	}

	if (this->HandleSpecialCommand(szCommand, szArgs))
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SaveCVarToProfile(const char* key, const string& value)
{
	if (!m_pPlayerProfileManager)
		return;
	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile)
		return;

	pProfile->SetAttribute(key, value);
	IPlayerProfileManager::EProfileOperationResult result;
	m_pPlayerProfileManager->SaveProfile(m_pPlayerProfileManager->GetCurrentUser(), result);
}

//-----------------------------------------------------------------------------------------------------

bool COptionsManager::GetProfileValue(const char* key, int& value)
{
	if (!m_pPlayerProfileManager) return false;

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile) return false;

	return pProfile->GetAttribute(key, value);
}
//-----------------------------------------------------------------------------------------------------

bool COptionsManager::GetProfileValue(const char* key, bool& value)
{
	if (!m_pPlayerProfileManager) return false;

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile) return false;

	return pProfile->GetAttribute(key, value);
}


//-----------------------------------------------------------------------------------------------------

bool COptionsManager::GetProfileValue(const char* key, float& value)
{
	if (!m_pPlayerProfileManager) return false;

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile) return false;

	return pProfile->GetAttribute(key, value);
}

//-----------------------------------------------------------------------------------------------------

bool COptionsManager::GetProfileValue(const char* key, string& value)
{
	if (!m_pPlayerProfileManager)
	{
		if (gEnv->pSystem->IsEditor())
		{
			if (strcmp(key, "ColorLine") == 0)
			{
				value = m_defaultColorLine;
			}
			else if (strcmp(key, "ColorOver") == 0)
			{
				value = m_defaultColorOver;
			}
			else if (strcmp(key, "ColorText") == 0)
			{
				value = m_defaultColorText;
			}
			else
			{
				value.clear();
			}
			return true;
		}
		return false;
	}

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile) return false;

	return pProfile->GetAttribute(key, value);
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SaveValueToProfile(const char* key, const string& value)
{
	if (!m_pPlayerProfileManager)
		return;

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile)
		return;

	pProfile->SetAttribute(key, value);
	SaveProfile();
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SaveValueToProfile(const char* key, bool value)
{
	if (!m_pPlayerProfileManager)
		return;

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile)
		return;

	pProfile->SetAttribute(key, value);
	SaveProfile();
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SaveValueToProfile(const char* key, int value)
{
	if (!m_pPlayerProfileManager)
		return;

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile)
		return;

	pProfile->SetAttribute(key, value);
	SaveProfile();
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SaveValueToProfile(const char* key, float value)
{
	if (!m_pPlayerProfileManager)
		return;

	IPlayerProfile* pProfile = m_pPlayerProfileManager->GetCurrentProfile(m_pPlayerProfileManager->GetCurrentUser());
	if (!pProfile)
		return;

	pProfile->SetAttribute(key, value);
	SaveProfile();
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SaveProfile()
{
	if (!m_pPlayerProfileManager)
		return;
	IPlayerProfileManager::EProfileOperationResult result;
	m_pPlayerProfileManager->SaveProfile(m_pPlayerProfileManager->GetCurrentUser(), result);
	WriteGameCfg();
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::AutoDetectHardware(const char* params)
{
	GetISystem()->AutoDetectSpec();
	SystemConfigChanged();
}
//-----------------------------------------------------------------------------------------------------

void COptionsManager::SetVideoMode(const char* params)
{
	CryFixedStringT<64> resolution(params);
	int pos = resolution.find('x');
	if (pos != CryFixedStringT<64>::npos)
	{
		CryFixedStringT<64> width("r_width ");
		width.append(resolution.substr(0, pos));
		CryFixedStringT<64> height("r_height ");
		height.append(resolution.substr(pos + 1, resolution.size()));

		g_pGame->GetMenu()->StartResolutionCountDown();
		gEnv->pConsole->ExecuteString(width.c_str());
		gEnv->pConsole->ExecuteString(height.c_str());
	}
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SetAntiAliasingMode(const char* params)
{
	if (params)
	{
		if (g_pGame->GetMenu())
		{
			CFlashMenuObject::FSAAMode mode = g_pGame->GetMenu()->GetFSAAMode(params);
			if (mode.samples == 0)
			{
				gEnv->pConsole->ExecuteString("r_fsaa 0");
				gEnv->pConsole->ExecuteString("r_fsaa_samples 0");
				gEnv->pConsole->ExecuteString("r_fsaa_quality 0");
			}
			else
			{
				gEnv->pConsole->ExecuteString("r_fsaa 1");
				CryFixedStringT<32> command("r_fsaa_samples ");
				char buffer[16];
				_itoa(mode.samples, buffer, 10);
				command.append(buffer);
				gEnv->pConsole->ExecuteString(command.c_str());
				command.assign("r_fsaa_quality ");
				_itoa(mode.quality, buffer, 10);
				command.append(buffer);
				gEnv->pConsole->ExecuteString(command.c_str());

				// FSAA requires HDR mode on, to get consistent menu settings we switch sys_spec_shading to 3 or 4
				// search for #LABEL_FSAA_HDR
				{
					bool bHDREnabled = gEnv->pRenderer->EF_Query(EFQ_HDRModeEnabled) != 0;

					if (!bHDREnabled)		// no HDR so we either have sys_spec_Shading in 1 or 2 or user
					{										// (it cannot be the machine is not capable of HDR as we have a list of FSAA modes)
						ICVar* pSpecShading = gEnv->pConsole->GetCVar("sys_spec_Shading");			assert(pSpecShading);

						if (pSpecShading)
							pSpecShading->Set(3);		// starting with mode 3 we have HDR on
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SetDifficulty(const char* params)
{
	if (params && params[0])
		g_pGame->GetMenu()->LoadDifficultyConfig(atoi(params));
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::PBClient(const char* params)
{
	if (params)
	{
		int value = atoi(params);
		if (value == 0)
		{
			m_pbEnabled = false;
			gEnv->pConsole->ExecuteString("net_pb_cl_enable false");
		}
		else
		{
			m_pbEnabled = true;
			gEnv->pConsole->ExecuteString("net_pb_cl_enable true");
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void COptionsManager::SystemConfigChanged(bool silent)
{
	//gEnv->pConsole->ExecuteString("sys_SaveCVars 1");
	//gEnv->pSystem->SaveConfiguration();
	//gEnv->pConsole->ExecuteString("sys_SaveCVars 0");

	if (m_pPlayerProfileManager)
	{
		UpdateToProfile();
		SaveProfile();
	}

	if (!silent)
	{
		if (CFlashMenuScreen* pCurrentMenu = GetCurrentMenu())
		{
			pCurrentMenu->Invoke("showErrorMessage", "Box1");
			CryFixedStringT<128> text("@system_spec_");
			text.append(gEnv->pConsole->GetCVar("sys_spec")->GetString());
			pCurrentMenu->Invoke("setErrorText", text.c_str());
			text.assign("sys_spec_Full ");
			text.append(gEnv->pConsole->GetCVar("sys_spec")->GetString());
			gEnv->pConsole->ExecuteString(text.c_str());
		}
	}
	UpdateFlashOptions();

}

//-----------------------------------------------------------------------------------------------------

bool COptionsManager::HandleSpecialCommand(std::string_view command, const char* args)
{
	if (command == "SetVideoMode")
	{
		this->SetVideoMode(args);
		return true;
	}
	else if (command == "SetAntiAliasingMode")
	{
		this->SetAntiAliasingMode(args);
		return true;
	}
	else if (command == "AutoDetectHardware")
	{
		this->AutoDetectHardware(args);
		return true;
	}
	else if (command == "g_difficultyLevel")
	{
		this->SetDifficulty(args);
		return true;
	}
	else if (command == "pb_client")
	{
		this->PBClient(args);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------

CFlashMenuScreen* COptionsManager::GetCurrentMenu()
{
	CFlashMenuScreen* pMainMenu = SAFE_MENU_FUNC_RET(GetMenuScreen(CFlashMenuObject::MENUSCREEN_FRONTENDSTART));
	CFlashMenuScreen* pInGameMenu = SAFE_MENU_FUNC_RET(GetMenuScreen(CFlashMenuObject::MENUSCREEN_FRONTENDINGAME));

	CFlashMenuScreen* pCurrentMenu = NULL;
	if (pMainMenu && pMainMenu->IsLoaded())
		pCurrentMenu = pMainMenu;
	else if (pInGameMenu && pInGameMenu->IsLoaded())
		pCurrentMenu = pInGameMenu;

	return pCurrentMenu;
}

//-----------------------------------------------------------------------------------------------------

static void EscapeChar(std::string& text, char ch)
{
	for (auto it = text.begin(); it != text.end(); ++it)
	{
		if (*it == ch)
		{
			it = text.insert(it, '\\');
			++it;
		}
	}
}

bool COptionsManager::WriteGameCfg()
{
	CryFile file("%USER%/game.cfg", "w");
	if (!file)
	{
		CryLogErrorAlways("Failed to open %USER%/game.cfg for writing!");
		return false;
	}

	file.Puts("-- [Game-Configuration]");
	file.Puts("-- Attention: This file is re-generated by the system! Editing is not recommended!");
	file.Puts("");

	CCVarSink sink(this, file);
	gEnv->pConsole->DumpCVars(&sink);

	return true;
}

void COptionsManager::CCVarSink::OnElementFound(ICVar* pCVar)
{
	if (!pCVar)
	{
		return;
	}

	auto it = this->self->m_profileOptions.find(pCVar->GetName());
	if (it == this->self->m_profileOptions.end())
	{
		return;
	}

	if (!it->second.writeToConfig)
	{
		return;
	}

	std::string value = pCVar->GetString();
	EscapeChar(value, '\\');
	EscapeChar(value, '"');

	if (pCVar->GetType() == CVAR_STRING)
	{
		this->file.FPrintf("%s = \"%s\"\n", pCVar->GetName(), value.c_str());
	}
	else
	{
		this->file.FPrintf("%s = %s\n", pCVar->GetName(), value.c_str());
	}
}
