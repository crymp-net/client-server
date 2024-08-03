// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __GAMECVARS_H__
#define __GAMECVARS_H__

struct IConsole;
struct ICVar;

struct SCVars
{
	IConsole* pConsole = gEnv->pConsole;

	static const float v_altitudeLimitDefault()
	{
		return 600.0f;
	}
	ICVar*	ca_GameControlledStrafingPtr;
	ICVar*	pl_debug_filter;
	ICVar*	aln_debug_filter;
	int		aln_debug_movement;
	float	v_altitudeLimit;
	float	v_altitudeLimitLowerOffset;
	ICVar*	pAltitudeLimitCVar;
	ICVar*	pAltitudeLimitLowerOffsetCVar;
	ICVar*	pVehicleQuality;
	ICVar*	i_debuggun_1;
	ICVar*	i_debuggun_2;
	ICVar*	g_quickGame_map;
	ICVar*	g_quickGame_mode;
	ICVar*	g_battleDust_effect;

	int		goc_enable;
	int		bt_speed;
	int		bt_ironsight;
	int		bt_end_reload;
	int		bt_end_select;
	int		bt_end_melee;
	float	bt_time_scale;
	float	bt_pitch;
	float	bt_energy_max;
	float	bt_energy_decay;
	float	bt_energy_regen;

	#define STRUCT_GAMECVAR
		#include "GameCvarsLib.h"
	#undef STRUCT_GAMECVAR

	SCVars();
	~SCVars();
};

extern struct SCVars *g_pGameCVars;

#endif //__GAMECVARS_H__
