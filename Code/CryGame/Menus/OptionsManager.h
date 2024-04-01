/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Handles options setting, getting, saving and loading
between profile, system and flash.

-------------------------------------------------------------------------
History:
- 03/2007: Created by Jan Müller

*************************************************************************/
#pragma once

#include <map>
#include <string>
#include <string_view>

#include "CryCommon/CrySystem/IConsole.h"

//-----------------------------------------------------------------------------------------------------

class CCryFile;
class CFlashMenuScreen;
struct IPlayerProfileManager;

//-----------------------------------------------------------------------------------------------------

enum ECrysisProfileColor
{
	CrysisProfileColor_Amber,
	CrysisProfileColor_Blue,
	CrysisProfileColor_Green,
	CrysisProfileColor_Red,
	CrysisProfileColor_White,
	CrysisProfileColor_Default = CrysisProfileColor_Green
};

//-----------------------------------------------------------------------------------------------------

class COptionsManager
{
public:

	~COptionsManager() 
	{
		sp_optionsManager = 0;
	};

	static COptionsManager* CreateOptionsManager()
	{
		if(!sp_optionsManager)
			sp_optionsManager = new COptionsManager();
		return sp_optionsManager;
	}

	IPlayerProfileManager* GetProfileManager() { return m_pPlayerProfileManager; }
	bool IsFirstStart() { return m_firstStart; }

	ECrysisProfileColor GetCrysisProfileColor();

	//flash system
	bool HandleFSCommand(const char *strCommand,const char *strArgs);
	void UpdateFlashOptions();
	void InitProfileOptions(bool switchProfiles = false);
	void UpdateToProfile();
	void ResetDefaults(const char* option);
	//flash system

	//profile system
	void SetProfileManager(IPlayerProfileManager* pProfileManager);
	void SaveCVarToProfile(const char* key, const string& value);
	bool GetProfileValue(const char* key, bool &value);
	bool GetProfileValue(const char* key, int &value);
	bool GetProfileValue(const char* key, float &value);
	bool GetProfileValue(const char* key, string &value);
	void SaveValueToProfile(const char* key, bool value);
	void SaveValueToProfile(const char* key, int value);
	void SaveValueToProfile(const char* key, float value);
	void SaveValueToProfile(const char* key, const string& value);
	void SaveProfile();
	const char* GetProfileName();
	void CVarToProfile();
	void ProfileToCVar();
	bool IgnoreProfile();
	bool WriteGameCfg();
	void SetVideoMode(const char* params);
	void SetAntiAliasingMode(const char* params);
	void SetDifficulty(const char* params);
	void PBClient(const char* params);
	void AutoDetectHardware(const char* params);
	void SystemConfigChanged(bool silent = false);
	//~profile system

private:
	bool HandleSpecialCommand(std::string_view command, const char* args);

	struct CCVarSink : public ICVarDumpSink
	{
		const COptionsManager* self;
		CCryFile& file;

		explicit CCVarSink(const COptionsManager* self, CCryFile& file) : self(self), file(file) {}

		void OnElementFound(ICVar* pCVar) override;
	};

	COptionsManager();
	IPlayerProfileManager* m_pPlayerProfileManager;

	struct SOptionEntry
	{
		std::string name;
		bool writeToConfig = false;
	};

	std::map<std::string, SOptionEntry, std::less<void>> m_profileOptions;

	ECrysisProfileColor m_eCrysisProfileColor;

	void SetCrysisProfileColor(const char *szValue);
	CFlashMenuScreen * GetCurrentMenu();

	static COptionsManager* sp_optionsManager;

	const char *m_defaultColorLine;
	const char *m_defaultColorOver;
	const char *m_defaultColorText;
	bool m_pbEnabled;
	bool m_firstStart;

};

//-----------------------------------------------------------------------------------------------------
