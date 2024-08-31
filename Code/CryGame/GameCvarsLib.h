#ifndef __GAMECVARSLIB_H__
#define __GAMECVARSLIB_H__

#define GAMECVAR( varType, name, pValue, defaultValue, flags, help, pChangeFunc ) 

#ifdef STRUCT_GAMECVAR
	#undef GAMECVAR
	#define GAMECVAR( varType, name, pValue, defaultValue, flags, help, pChangeFunc ) varType pValue
#endif

#ifdef GAMECVAR_REGISTERLIST
	#undef GAMECVAR
	#define GAMECVAR( varType, name, pValue, defaultValue, flags, help, pChangeFunc ) ICVar* p_##pValue = pConsole->Register( name, & pValue, defaultValue, flags, help, pChangeFunc )
#endif

//client cvars
GAMECVAR(int, "cl_hud", cl_hud, 1, 0, "Show/Hide the HUD", CHUDCommon::HUD);
GAMECVAR(float, "cl_fov", cl_fov, 60.0f, 0, "field of view.", NULL);
GAMECVAR(float, "cl_bob", cl_bob, 1.0f, 0, "view/weapon bobbing multiplier", NULL);
GAMECVAR(float, "cl_headBob", cl_headBob, 1.0f, 0, "head bobbing multiplier", NULL);
GAMECVAR(float, "cl_headBobLimit", cl_headBobLimit, 0.06f, 0, "head bobbing distance limit", NULL);
GAMECVAR(float, "cl_tpvDist", cl_tpvDist, 3.5f, 0, "camera distance in 3rd person view", NULL);
GAMECVAR(float, "cl_tpvYaw", cl_tpvYaw, 0, 0, "camera angle offset in 3rd person view", NULL);
GAMECVAR(float, "cl_nearPlane", cl_nearPlane, 0, 0, "overrides the near clipping plane if != 0, just for testing.", NULL);
GAMECVAR(float, "cl_sprintShake", cl_sprintShake, 0.0f, 0, "sprint shake", NULL);
GAMECVAR(float, "cl_sensitivityZeroG", cl_sensitivityZeroG, 70.0f, VF_DUMPTODISK, "Set mouse sensitivity in ZeroG!", NULL);
GAMECVAR(float, "cl_sensitivity", cl_sensitivity, 45.0f, VF_DUMPTODISK, "Set mouse sensitivity!", NULL);
GAMECVAR(float, "cl_controllersensitivity", cl_controllersensitivity, 45.0f, VF_DUMPTODISK, "Set controller sensitivity!", NULL);
GAMECVAR(int, "cl_invertMouse", cl_invertMouse, 0, VF_DUMPTODISK, "mouse invert?", NULL);
GAMECVAR(int, "cl_invertController", cl_invertController, 0, VF_DUMPTODISK, "Controller Look Up-Down invert", NULL);
GAMECVAR(int, "cl_crouchToggle", cl_crouchToggle, 0, VF_DUMPTODISK, "To make the crouch key work as a toggle", NULL);
GAMECVAR(int, "cl_fpBody", cl_fpBody, 2, 0, "first person body", NULL);
//FIXME:just for testing
GAMECVAR(int, "cl_strengthscale", cl_strengthscale, 1.0f, 0, "nanosuit strength scale", NULL);

//CryMP: enable improved TP camera (might remove some of these later)
GAMECVAR(int, "goc_tpcrosshair", goc_tpcrosshair, 0, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "keep crosshair in third person", NULL);
GAMECVAR(float, "goc_targetx", goc_targetx, 0.5f, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "target position of camera", NULL);
GAMECVAR(float, "goc_targety", goc_targety, -3.5f, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "target position of camera", NULL);
GAMECVAR(float, "goc_targetz", goc_targetz, 0.2f, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "target position of camera", NULL);

GAMECVAR(float, "cl_leanAmount", cl_leanAmount, 0.25f, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "set amount of lean", NULL);

/*
GAMECVAR(int, "goc_enable", goc_enable, 0, VF_CHEAT, "gears of crysis", NULL);
pConsole->AddCommand("GOCMode", CmdGOCMode, VF_CHEAT, "Enable GOC mode");

// BulletTime
GAMECVAR(int, "bt_speed", bt_speed, 0, VF_CHEAT, "bullet-time when in speed mode", NULL);
GAMECVAR(int, "bt_ironsight", bt_ironsight, 0, VF_CHEAT, "bullet-time when in ironsight", NULL);
GAMECVAR(int, "bt_end_reload", bt_end_reload, 0, VF_CHEAT, "end bullet-time when reloading", NULL);
GAMECVAR(int, "bt_end_select", bt_end_select, 0, VF_CHEAT, "end bullet-time when selecting a new weapon", NULL);
GAMECVAR(int, "bt_end_melee", bt_end_melee, 0, VF_CHEAT, "end bullet-time when melee", NULL);
GAMECVAR(float, "bt_time_scale", bt_time_scale, 0.2f, VF_CHEAT, "bullet-time time scale to apply", NULL);
GAMECVAR(float, "bt_pitch", bt_pitch, -0.4f, VF_CHEAT, "sound pitch shift for bullet-time", NULL);
GAMECVAR(float, "bt_energy_max", bt_energy_max, 1.0f, VF_CHEAT, "maximum bullet-time energy", NULL);
GAMECVAR(float, "bt_energy_decay", bt_energy_decay, 2.5f, VF_CHEAT, "bullet time energy decay rate", NULL);
GAMECVAR(float, "bt_energy_regen", bt_energy_regen, 0.5f, VF_CHEAT, "bullet time energy regeneration rate", NULL);
pConsole->AddCommand("bulletTimeMode", CmdBulletTimeMode, VF_CHEAT, "Enable bullet time mode");
*/

GAMECVAR(int, "dt_enable", dt_enable, 0, 0, "suit actions activated by double-tapping", NULL);
GAMECVAR(float, "dt_time", dt_time, 0.25f, 0, "time in seconds between double taps", NULL);
GAMECVAR(float, "dt_meleeTime", dt_meleeTime, 0.3f, 0, "time in seconds between double taps for melee", NULL);

GAMECVAR(int, "i_staticfiresounds", i_staticfiresounds, 1, VF_DUMPTODISK, "Enable/Disable static fire sounds. Static sounds are not unloaded when idle.", NULL);
GAMECVAR(int, "i_soundeffects", i_soundeffects, 1, VF_DUMPTODISK, "Enable/Disable playing item sound effects.", NULL);
GAMECVAR(int, "i_lighteffects", i_lighteffects, 1, VF_DUMPTODISK, "Enable/Disable lights spawned during item effects.", NULL);
GAMECVAR(int, "i_particleeffects", i_particleeffects, 1, VF_DUMPTODISK, "Enable/Disable particles spawned during item effects.", NULL);
GAMECVAR(int, "i_rejecteffects", i_rejecteffects, 1, VF_DUMPTODISK, "Enable/Disable ammo reject effects during weapon firing.", NULL);
GAMECVAR(float, "i_offset_front", i_offset_front, 0.0f, 0, "Item position front offset", NULL);
GAMECVAR(float, "i_offset_up", i_offset_up, 0.0f, 0, "Item position up offset", NULL);
GAMECVAR(float, "i_offset_right", i_offset_right, 0.0f, 0, "Item position right offset", NULL);
GAMECVAR(int, "i_unlimitedammo", i_unlimitedammo, 0, VF_CHEAT, "unlimited ammo", NULL);
GAMECVAR(int, "i_iceeffects", i_iceeffects, 0, VF_CHEAT, "Enable/Disable specific weapon effects for ice environments", NULL);

GAMECVAR(int, "i_lighteffectShadows", i_lighteffectsShadows, 0, VF_DUMPTODISK, "Enable/Disable shadow casting on weapon lights. 1 - Player only, 2 - Other players/AI, 3 - All (require i_lighteffects enabled).", NULL);

GAMECVAR(int, "cl_debugSwimming", cl_debugSwimming, 0, VF_CHEAT, "enable swimming debugging", NULL);

GAMECVAR(float, "pl_curvingSlowdownSpeedScale", pl_curvingSlowdownSpeedScale, 0.5f, VF_CHEAT, "Player only slowdown speedscale when curving/leaning extremely.", NULL);
GAMECVAR(float, "ac_enableProceduralLeaning", ac_enableProceduralLeaning, 1.0f, VF_CHEAT, "Enable procedural leaning (disabled asset leaning and curving slowdown).", NULL);

GAMECVAR(float, "cl_shallowWaterSpeedMulPlayer", cl_shallowWaterSpeedMulPlayer, 0.6f, VF_CHEAT, "shallow water speed multiplier (Players only)", NULL);
GAMECVAR(float, "cl_shallowWaterSpeedMulAI", cl_shallowWaterSpeedMulAI, 0.8f, VF_CHEAT, "Shallow water speed multiplier (AI only)", NULL);
GAMECVAR(float, "cl_shallowWaterDepthLo", cl_shallowWaterDepthLo, 0.3f, VF_CHEAT, "Shallow water depth low (below has zero slowdown)", NULL);
GAMECVAR(float, "cl_shallowWaterDepthHi", cl_shallowWaterDepthHi, 1.0f, VF_CHEAT, "Shallow water depth high (above has full slowdown)", NULL);


GAMECVAR(float, "pl_inputAccel", pl_inputAccel, 30.0f, 0, "Movement input acceleration", NULL);


GAMECVAR(int, "h_useIK", h_useIK, 1, 0, "Hunter uses always IK", NULL);
GAMECVAR(int, "h_drawSlippers", h_drawSlippers, 0, 0, "Red ball when tentacle is lifted, green when on ground", NULL);
GAMECVAR(float, "g_tentacle_joint_limit", g_tentacle_joint_limit, -1.0f, 0, "forces specific tentacle limits; used for tweaking", NULL);
GAMECVAR(int, "g_enableSpeedLean", g_enableSpeedLean, 0, 0, "Enables player-controlled curve leaning in speed mode.", NULL);
//
GAMECVAR(float, "int_zoomAmount", int_zoomAmount, 0.75f, VF_CHEAT, "Maximum zoom, between 0.0 and 1.0. Default = .75", NULL);
GAMECVAR(float, "int_zoomInTime", int_zoomInTime, 5.0f, VF_CHEAT, "Number of seconds it takes to zoom in. Default = 5.0", NULL);
GAMECVAR(float, "int_moveZoomTime", int_moveZoomTime, 0.1f, VF_CHEAT, "Number of seconds it takes to zoom out when moving. Default = 0.2", NULL);
GAMECVAR(float, "int_zoomOutTime", int_zoomOutTime, 0.1f, VF_CHEAT, "Number of seconds it takes to zoom out when you stop firing. Default = 0.5", NULL);


GAMECVAR(float, "hr_rotateFactor", hr_rotateFactor, -.1f, VF_CHEAT, "rotate factor", NULL);
GAMECVAR(float, "hr_rotateTime", hr_rotateTime, .07f, VF_CHEAT, "rotate time", NULL);
GAMECVAR(float, "hr_dotAngle", hr_dotAngle, .75f, VF_CHEAT, "max angle for FOV change", NULL);
GAMECVAR(float, "hr_fovAmt", hr_fovAmt, .03f, VF_CHEAT, "goal FOV when hit", NULL);
GAMECVAR(float, "hr_fovTime", hr_fovTime, .05f, VF_CHEAT, "fov time", NULL);

// frozen shake vars (for tweaking only)
GAMECVAR(float, "cl_debugFreezeShake", cl_debugFreezeShake, 0, VF_CHEAT | VF_DUMPTODISK, "Toggle freeze shake debug draw", NULL);
GAMECVAR(float, "cl_frozenSteps", cl_frozenSteps, 3, VF_CHEAT, "Number of steps for unfreeze shaking", NULL);
GAMECVAR(float, "cl_frozenSensMin", cl_frozenSensMin, 1.0f, VF_CHEAT, "Frozen sensitivity min", NULL); // was 0.2
GAMECVAR(float, "cl_frozenSensMax", cl_frozenSensMax, 1.0f, VF_CHEAT, "Frozen sensitivity max", NULL); // was 0.4
GAMECVAR(float, "cl_frozenAngleMin", cl_frozenAngleMin, 1.f, VF_CHEAT, "Frozen clamp angle min", NULL);
GAMECVAR(float, "cl_frozenAngleMax", cl_frozenAngleMax, 10.f, VF_CHEAT, "Frozen clamp angle max", NULL);
GAMECVAR(float, "cl_frozenMouseMult", cl_frozenMouseMult, 0.00015f, VF_CHEAT, "Frozen mouseshake multiplier", NULL);
GAMECVAR(float, "cl_frozenKeyMult", cl_frozenKeyMult, 0.02f, VF_CHEAT, "Frozen movement keys multiplier", NULL);
GAMECVAR(float, "cl_frozenSoundDelta", cl_frozenSoundDelta, 0.004f, VF_CHEAT, "Threshold for unfreeze shake to trigger a crack sound", NULL);

GAMECVAR(float, "g_frostDecay", g_frostDecay, 0.25f, VF_CHEAT, "Frost decay speed when freezing actors", NULL);

GAMECVAR(float, "g_stanceTransitionSpeed", g_stanceTransitionSpeed, 15.0f, VF_CHEAT, "Set speed of camera transition from stance to stance", NULL);
GAMECVAR(float, "g_stanceTransitionSpeedSecondary", g_stanceTransitionSpeedSecondary, 6.0f, VF_CHEAT, "Set speed of camera transition from stance to stance", NULL);

GAMECVAR(int, "g_playerHealthValue", g_playerHealthValue, 100, VF_CHEAT, "Maximum player health.", NULL);
GAMECVAR(float, "g_walkMultiplier", g_walkMultiplier, 1, VF_SAVEGAME, "Modify movement speed", NULL);
GAMECVAR(float, "g_suitRecoilEnergyCost", g_suitRecoilEnergyCost, 15.0f, VF_CHEAT, "Subtracted energy when weapon is fired in strength mode.", NULL);
GAMECVAR(float, "g_suitSpeedMult", g_suitSpeedMult, 1.85f, 0, "Modify speed mode effect.", NULL);
GAMECVAR(float, "g_suitSpeedMultMultiplayer", g_suitSpeedMultMultiplayer, 0.35f, 0, "Modify speed mode effect for Multiplayer.", NULL);
GAMECVAR(float, "g_suitArmorHealthValue", g_suitArmorHealthValue, 200.0f, 0, "This defines how much damage is reduced by 100% energy, not considering recharge. The value should be between 1 and <SuitMaxEnergy>.", NULL);
GAMECVAR(float, "g_suitSpeedEnergyConsumption", g_suitSpeedEnergyConsumption, 110.0f, 0, "Energy reduction in speed mode per second.", NULL);
GAMECVAR(float, "g_suitSpeedEnergyConsumptionMultiplayer", g_suitSpeedEnergyConsumptionMultiplayer, 50.0f, 0, "Energy reduction in speed mode per second in multiplayer.", NULL);
GAMECVAR(float, "g_suitCloakEnergyDrainAdjuster", g_suitCloakEnergyDrainAdjuster, 1.0f, 0, "Multiplier for energy reduction in cloak mode.", NULL);
GAMECVAR(int, "g_mpSpeedRechargeDelay", g_mpSpeedRechargeDelay, 1, VF_CHEAT, "Toggles delay when sprinting below 20% energy.", NULL);
GAMECVAR(float, "g_AiSuitEnergyRechargeTime", g_AiSuitEnergyRechargeTime, 10.0f, VF_CHEAT, "Modify suit energy recharge for AI.", NULL);
GAMECVAR(float, "g_AiSuitStrengthMeleeMult", g_AiSuitStrengthMeleeMult, 0.4f, VF_CHEAT, "Modify AI strength mode melee damage relative to player damage.", NULL);
GAMECVAR(float, "g_AiSuitHealthRegenTime", g_AiSuitHealthRegenTime, 33.3f, VF_CHEAT, "Modify suit health recharge for AI.", NULL);
GAMECVAR(float, "g_AiSuitArmorModeHealthRegenTime", g_AiSuitArmorModeHealthRegenTime, 20.0f, VF_CHEAT, "Modify suit health recharge for AI in armor mode.", NULL);
GAMECVAR(float, "g_playerSuitEnergyRechargeTime", g_playerSuitEnergyRechargeTime, 10.0f, VF_CHEAT, "Modify suit energy recharge for Player.", NULL);
GAMECVAR(float, "g_playerSuitEnergyRechargeTimeArmor", g_playerSuitEnergyRechargeTimeArmor, 10.0f, VF_CHEAT, "Modify suit energy recharge for Player in singleplayer in armor mode.", NULL);
GAMECVAR(float, "g_playerSuitEnergyRechargeTimeArmorMoving", g_playerSuitEnergyRechargeTimeArmorMoving, 10.0f, VF_CHEAT, "Modify suit energy recharge for Player in singleplayer in armor mode while moving.", NULL);
GAMECVAR(float, "g_playerSuitEnergyRechargeTimeMultiplayer", g_playerSuitEnergyRechargeTimeMultiplayer, 20.0f, VF_CHEAT, "Modify suit energy recharge for Player in multiplayer.", NULL);
GAMECVAR(float, "g_playerSuitEnergyRechargeDelay", g_playerSuitEnergyRechargeDelay, 0.0f, VF_CHEAT, "Delay of energy recharge after the player has been hit.", NULL);
GAMECVAR(float, "g_playerSuitHealthRegenTime", g_playerSuitHealthRegenTime, 35.0f, VF_CHEAT, "Modify suit health recharge for Player.", NULL);
GAMECVAR(float, "g_playerSuitHealthRegenTimeMoving", g_playerSuitHealthRegenTimeMoving, 35.0f, VF_CHEAT, "Modify suit health recharge for moving Player.", NULL);
GAMECVAR(float, "g_playerSuitArmorModeHealthRegenTime", g_playerSuitArmorModeHealthRegenTime, 15.0f, VF_CHEAT, "Modify suit health recharge for Player in armor mode.", NULL);
GAMECVAR(float, "g_playerSuitArmorModeHealthRegenTimeMoving", g_playerSuitArmorModeHealthRegenTimeMoving, 30.0f, VF_CHEAT, "Modify suit health recharge for Player moving in armor mode.", NULL);
GAMECVAR(float, "g_playerSuitHealthRegenDelay", g_playerSuitHealthRegenDelay, 0.0f, VF_CHEAT, "Delay of health regeneration after the player has been hit.", NULL);
GAMECVAR(int, "g_difficultyLevel", g_difficultyLevel, 0, VF_CHEAT | VF_READONLY, "Difficulty level", NULL);
GAMECVAR(int, "g_difficultyHintSystem", g_difficultyHintSystem, 2, VF_CHEAT | VF_READONLY, "Lower difficulty hint system (0 is off, 1 is radius based, 2 is save-game based)", NULL);
GAMECVAR(float, "g_difficultyRadius", g_difficultyRadius, 300, VF_CHEAT | VF_READONLY, "Radius in which player needs to die to display lower difficulty level hint.", NULL);
GAMECVAR(int, "g_difficultyRadiusThreshold", g_difficultyRadiusThreshold, 5, VF_CHEAT | VF_READONLY, "Number of times player has to die within radius to trigger difficulty hint.", NULL);
GAMECVAR(int, "g_difficultySaveThreshold", g_difficultySaveThreshold, 5, VF_CHEAT | VF_READONLY, "Number of times player has to die with same savegame active to trigger difficulty hint.", NULL);
GAMECVAR(float, "g_pp_scale_income", g_pp_scale_income, 1, 0, "Scales incoming PP.", NULL);
GAMECVAR(float, "g_pp_scale_price", g_pp_scale_price, 1, 0, "Scales PP prices.", NULL);
GAMECVAR(float, "g_energy_scale_price", g_energy_scale_price, 0, 0, "Scales energy prices.", NULL);
GAMECVAR(float, "g_energy_scale_income", g_energy_scale_income, 1, 0, "Scales incoming energy.", NULL);
GAMECVAR(int, "g_enableFriendlyFallAndPlay", g_enableFriendlyFallAndPlay, 0, 0, "Enables fallplay feedback for friendly actors.", NULL);

GAMECVAR(int, "g_playerRespawns", g_playerRespawns, 0, VF_SAVEGAME, "Sets the player lives.", NULL);
GAMECVAR(float, "g_playerLowHealthThreshold", g_playerLowHealthThreshold, 40.0f, VF_CHEAT, "The player health threshold when the low health effect kicks in.", NULL);
GAMECVAR(float, "g_playerLowHealthThreshold2", g_playerLowHealthThreshold2, 60.0f, VF_CHEAT, "The player health threshold when the low health effect reaches maximum.", NULL);
GAMECVAR(float, "g_playerLowHealthThresholdMultiplayer", g_playerLowHealthThresholdMultiplayer, 20.0f, VF_CHEAT, "The player health threshold when the low health effect kicks in.", NULL);
GAMECVAR(float, "g_playerLowHealthThreshold2Multiplayer", g_playerLowHealthThreshold2Multiplayer, 30.0f, VF_CHEAT, "The player health threshold when the low health effect reaches maximum.", NULL);
GAMECVAR(int, "g_punishFriendlyDeaths", g_punishFriendlyDeaths, 1, VF_CHEAT, "The player gets punished by death when killing a friendly unit.", NULL);
GAMECVAR(int, "g_enableMPStealthOMeter", g_enableMPStealthOMeter, 0, VF_CHEAT, "Enables the stealth-o-meter to detect enemies in MP matches.", NULL);
GAMECVAR(int, "g_meleeWhileSprinting", g_meleeWhileSprinting, 0, 0, "Enables option to melee while sprinting, using left mouse button.", NULL);
GAMECVAR(float, "g_fallAndPlayThreshold", g_fallAndPlayThreshold, 50, VF_CHEAT, "Minimum damage for fall and play.", NULL);

// Depth of Field control
GAMECVAR(float, "g_dofset_minScale", g_dofset_minScale, 1.0f, VF_CHEAT, "Scale Dof_FocusMin param when it gets set Default = 1", NULL);
GAMECVAR(float, "g_dofset_maxScale", g_dofset_maxScale, 3.0f, VF_CHEAT, "Scale Dof_FocusMax param when it gets set Default = 3", NULL);
GAMECVAR(float, "g_dofset_limitScale", g_dofset_limitScale, 9.0f, VF_CHEAT, "Scale Dof_FocusLimit param when it gets set Default = 9", NULL);
GAMECVAR(float, "g_dof_minHitScale", g_dof_minHitScale, 0.25f, VF_CHEAT, "Scale of ray hit distance which Min tries to approach. Default = 0.25", NULL);
GAMECVAR(float, "g_dof_maxHitScale", g_dof_maxHitScale, 2.0f, VF_CHEAT, "Scale of ray hit distance which Max tries to approach. Default = 2.0f", NULL);
GAMECVAR(float, "g_dof_sampleAngle", g_dof_sampleAngle, 5.0f, VF_CHEAT, "Sample angle in degrees. Default = 5", NULL);
GAMECVAR(float, "g_dof_minAdjustSpeed", g_dof_minAdjustSpeed, 100.0f, VF_CHEAT, "Speed that DoF can adjust the min value with. Default = 100", NULL);
GAMECVAR(float, "g_dof_maxAdjustSpeed", g_dof_maxAdjustSpeed, 200.0f, VF_CHEAT, "Speed that DoF can adjust the max value with. Default = 200", NULL);
GAMECVAR(float, "g_dof_averageAdjustSpeed", g_dof_averageAdjustSpeed, 20.0f, VF_CHEAT, "Speed that the average between min and max can be approached. Default = 20", NULL);
GAMECVAR(float, "g_dof_distAppart", g_dof_distAppart, 10.0f, VF_CHEAT, "Minimum distance that max and min can be apart. Default = 10", NULL);
GAMECVAR(int, "g_dof_ironsight", g_dof_ironsight, 1, VF_CHEAT, "Enable ironsight dof. Default = 1", NULL);

// explosion culling
GAMECVAR(int, "g_ec_enable", g_ec_enable, 1, VF_CHEAT, "Enable/Disable explosion culling of small objects. Default = 1", NULL);
GAMECVAR(float, "g_ec_radiusScale", g_ec_radiusScale, 2.0f, VF_CHEAT, "Explosion culling scale to apply to explosion radius for object query.", NULL);
GAMECVAR(float, "g_ec_volume", g_ec_volume, 0.75f, VF_CHEAT, "Explosion culling volume which needs to be exceed for objects to not be culled.", NULL);
GAMECVAR(float, "g_ec_extent", g_ec_extent, 2.0f, VF_CHEAT, "Explosion culling length of an AABB side which needs to be exceed for objects to not be culled.", NULL);
GAMECVAR(int, "g_ec_removeThreshold", g_ec_removeThreshold, 20, VF_CHEAT, "At how many items in exploding area will it start removing items.", NULL);

GAMECVAR(float, "g_radialBlur", g_radialBlur, 1.0f, VF_CHEAT, "Radial blur on explosions. Default = 1, 0 to disable", NULL);
GAMECVAR(int, "g_playerFallAndPlay", g_playerFallAndPlay, 0, 0, "When enabled, the player doesn't die from direct damage, but goes to fall and play.", NULL);

GAMECVAR(int, "g_enableTracers", g_enableTracers, 1, 0, "Enable/Disable tracers.", NULL);
GAMECVAR(int, "g_enableAlternateIronSight", g_enableAlternateIronSight, 0, 0, "Enable/Disable alternate ironsight mode", NULL);
GAMECVAR(float, "g_ragdollMinTime", g_ragdollMinTime, 10.0f, 0, "minimum time in seconds that a ragdoll will be visible", NULL);
GAMECVAR(float, "g_ragdollUnseenTime", g_ragdollUnseenTime, 2.0f, 0, "time in seconds that the player has to look away from the ragdoll before it disappears", NULL);
GAMECVAR(float, "g_ragdollPollTime", g_ragdollPollTime, 0.5f, 0, "time in seconds where 'unseen' polling is done", NULL);
GAMECVAR(float, "g_ragdollDistance", g_ragdollDistance, 10.0f, 0, "distance in meters that the player has to be away from the ragdoll before it can disappear", NULL);
GAMECVAR(int, "g_debugaimlook", g_debugaimlook, 0, VF_CHEAT, "Debug aim/look direction", NULL);
GAMECVAR(int, "g_enableIdleCheck", g_enableIdleCheck, 1, 0, "", NULL);

// Crysis supported gamemode CVars
GAMECVAR(float, "g_timelimit", g_timelimit, 60.0f, 0, "Duration of a time-limited game (in minutes). Default is 0, 0 means no time-limit.", NULL);
GAMECVAR(float, "g_roundtime", g_roundtime, 2.0f, 0, "Duration of a round (in minutes). Default is 0, 0 means no time-limit.", NULL);
GAMECVAR(int, "g_preroundtime", g_preroundtime, 8, 0, "Frozen time before round starts. Default is 8, 0 Disables freeze time.", NULL);
GAMECVAR(int, "g_suddendeathtime", g_suddendeathtime, 30, 0, "Number of seconds before round end to start sudden death. Default if 30. 0 Disables sudden death.", NULL);
GAMECVAR(int, "g_roundlimit", g_roundlimit, 30, 0, "Maximum numbers of rounds to be played. Default is 0, 0 means no limit.", NULL);
GAMECVAR(int, "g_fraglimit", g_fraglimit, 0, 0, "Number of frags before a round restarts. Default is 0, 0 means no frag-limit.", NULL);
GAMECVAR(int, "g_fraglead", g_fraglead, 1, 0, "Number of frags a player has to be ahead of other players once g_fraglimit is reached. Default is 1.", NULL);
GAMECVAR(float, "g_friendlyfireratio", g_friendlyfireratio, 1.0f, 0, "Sets friendly damage ratio.", NULL);
GAMECVAR(int, "g_revivetime", g_revivetime, 20, 0, "Revive wave timer.", NULL);
GAMECVAR(int, "g_autoteambalance", g_autoteambalance, 1, 0, "Enables auto team balance.", NULL);
GAMECVAR(int, "g_autoteambalance_threshold", g_autoteambalance_threshold, 3, 0, "Sets the auto team balance player threshold.", NULL);

GAMECVAR(int, "g_minplayerlimit", g_minplayerlimit, 2, 0, "Minimum number of players to start a match.", NULL);
GAMECVAR(int, "g_minteamlimit", g_minteamlimit, 1, 0, "Minimum number of players in each team to start a match.", NULL);
GAMECVAR(int, "g_tk_punish", g_tk_punish, 1, 0, "Turns on punishment for team kills", NULL);
GAMECVAR(int, "g_tk_punish_limit", g_tk_punish_limit, 5, 0, "Number of team kills user will be banned for", NULL);
GAMECVAR(int, "g_teamlock", g_teamlock, 2, 0, "Number of players one team needs to have over the other, for the game to deny joining it. 0 disables.", NULL);
GAMECVAR(int, "g_useHitSoundFeedback", g_useHitSoundFeedback, 1, 0, "Switches hit readability feedback sounds on/off.", NULL);

GAMECVAR(int, "g_debugNetPlayerInput", g_debugNetPlayerInput, 0, VF_CHEAT, "Show some debug for player input", NULL);
GAMECVAR(int, "g_debug_fscommand", g_debug_fscommand, 0, 0, "Print incoming fscommands to console", NULL);
GAMECVAR(int, "g_debugDirectMPMenu", g_debugDirectMPMenu, 0, 0, "Jump directly to MP menu on application start.", NULL);
GAMECVAR(int, "g_skipIntro", g_skipIntro, 0, VF_CHEAT, "Skip all the intro videos.", NULL);
GAMECVAR(int, "g_resetActionmapOnStart", g_resetActionmapOnStart, 0, 0, "Resets Keyboard mapping on application start.", NULL);
GAMECVAR(int, "g_useProfile", g_useProfile, 1, 0, "Don't save anything to or load anything from profile.", NULL);
GAMECVAR(int, "g_startFirstTime", g_startFirstTime, 1, VF_DUMPTODISK, "1 before the game was started first time ever.", NULL);
GAMECVAR(float, "g_cutsceneSkipDelay", g_cutsceneSkipDelay, 0.0f, 0, "Skip Delay for Cutscenes.", NULL);
GAMECVAR(int, "g_enableAutoSave", g_enableAutoSave, 1, 0, "Switches all savegames created by Flowgraph (checkpoints). Does not affect user generated saves or levelstart savegames.", NULL);

//
GAMECVAR(float, "g_godMode", g_godMode, 0, VF_CHEAT, "God Mode", NULL);
GAMECVAR(float, "g_detachCamera", g_detachCamera, 0, VF_CHEAT, "Detach camera", NULL);

GAMECVAR(int, "g_debugCollisionDamage", g_debugCollisionDamage, 0, VF_DUMPTODISK, "Log collision damage", NULL);
GAMECVAR(int, "g_debugHits", g_debugHits, 0, VF_DUMPTODISK, "Log hits", NULL);
GAMECVAR(float, "g_trooperProneMinDistance", g_trooperProneMinDistance, 10, VF_DUMPTODISK, "Distance to move for trooper to switch to prone stance", NULL);
//	GAMECVAR( float, "g_trooperMaxPhysicsAnimBlend", g_trooperMaxPhysicAnimBlend, 0, VF_DUMPTODISK, "Max value for trooper tentacle dynamic physics/anim blending", NULL );
//	GAMECVAR( float, "g_trooperPhysicsAnimBlendSpeed", g_trooperPhysicAnimBlendSpeed, 100.f, VF_DUMPTODISK, "Trooper tentacle dynamic physics/anim blending speed", NULL );
//	GAMECVAR( float, "g_trooperPhysicsDampingAnim", g_trooperPhysicsDampingAnim, 0, VF_DUMPTODISK, "Trooper tentacle dynamic physics/damping anim", NULL );
GAMECVAR(float, "g_trooperTentacleAnimBlend", g_trooperTentacleAnimBlend, 0, VF_DUMPTODISK, "Trooper tentacle physic_anim blend (0..1) - overrides the physic_blend AG parameter when it's not 0", NULL);
GAMECVAR(float, "g_trooperBankingMultiplier", g_trooperBankingMultiplier, 1, VF_DUMPTODISK, "Trooper banking multiplier coeff (0..x)", NULL);
GAMECVAR(float, "g_alienPhysicsAnimRatio", g_alienPhysicsAnimRatio, 0.0f, VF_CHEAT, "", NULL);

GAMECVAR(int, "g_spRecordGameplay", g_spRecordGameplay, 0, 0, "Write sp gameplay information to harddrive.", NULL);
GAMECVAR(float, "g_spGameplayRecorderUpdateRate", g_spGameplayRecorderUpdateRate, 1.0f, 0, "Update-delta of gameplay recorder in seconds.", NULL);

GAMECVAR(int, "pl_debug_ladders", pl_debug_ladders, 0, VF_CHEAT, "", NULL);
GAMECVAR(int, "pl_debug_movement", pl_debug_movement, 0, VF_CHEAT, "", NULL);
GAMECVAR(int, "pl_debug_jumping", pl_debug_jumping, 0, VF_CHEAT, "", NULL);

// emp grenade
GAMECVAR(int, "g_emp_style", g_empStyle, 0, VF_CHEAT, "", NULL);
GAMECVAR(float, "g_emp_nanosuit_downtime", g_empNanosuitDowntime, 10.0f, VF_CHEAT, "Time that the nanosuit is deactivated after leaving the EMP field.", NULL);

// hud cvars
GAMECVAR(int, "hud_mpNamesDuration", hud_mpNamesDuration, 2, 0, "MP names will fade after this duration.", NULL);
GAMECVAR(int, "hud_mpNamesNearDistance", hud_mpNamesNearDistance, 1, 0, "MP names will be fully visible when nearer than this.", NULL);
GAMECVAR(int, "hud_mpNamesFarDistance", hud_mpNamesFarDistance, 100, 0, "MP names will be fully invisible when farther than this.", NULL);
GAMECVAR(int, "hud_onScreenNearDistance", hud_onScreenNearDistance, 10, 0, "On screen icons won't scale anymore, when nearer than this.", NULL);
GAMECVAR(int, "hud_onScreenFarDistance", hud_onScreenFarDistance, 500, 0, "On screen icons won't scale anymore, when farther than this.", NULL);
GAMECVAR(float, "hud_onScreenNearSize", hud_onScreenNearSize, 1.4f, 0, "On screen icon size when nearest.", NULL);
GAMECVAR(float, "hud_onScreenFarSize", hud_onScreenFarSize, 0.7f, 0, "On screen icon size when farthest.", NULL);
GAMECVAR(int, "hud_colorLine", hud_colorLine, 4481854, 0, "HUD line color.", NULL);
GAMECVAR(int, "hud_colorOver", hud_colorOver, 14125840, 0, "HUD hovered color.", NULL);
GAMECVAR(int, "hud_colorText", hud_colorText, 12386209, 0, "HUD text color.", NULL);
GAMECVAR(int, "hud_voicemode", hud_voicemode, 1, 0, "Usage of the voice when switching of Nanosuit mode.", NULL);
GAMECVAR(int, "hud_enableAlienInterference", hud_enableAlienInterference, 1, VF_SAVEGAME, "Switched the alien interference effect.", NULL);
GAMECVAR(float, "hud_alienInterferenceStrength", hud_alienInterferenceStrength, 0.8f, VF_SAVEGAME, "Scales alien interference effect strength.", NULL);
GAMECVAR(int, "hud_godFadeTime", hud_godFadeTime, 3, VF_CHEAT, "sets the fade time of the god mode message", NULL);
GAMECVAR(int, "hud_crosshair_enable", hud_crosshair_enable, 1, 0, "Toggles singleplayer crosshair visibility.", CHUD::OnCrosshairCVarChanged);
GAMECVAR(int, "hud_crosshair", hud_crosshair, 1, 0, "Select the crosshair (1-8)", CHUD::OnCrosshairCVarChanged);

//new crosshair spread code (Julien)
GAMECVAR(int, "hud_alternateCrosshairSpread", hud_iAlternateCrosshairSpread, 0, 0, "Switch new crosshair spread code on/off.", NULL);
GAMECVAR(float, "hud_alternateCrosshairSpreadCrouch", hud_fAlternateCrosshairSpreadCrouch, 12.0f, VF_CHEAT, "", NULL);
GAMECVAR(float, "hud_alternateCrosshairSpreadNeutral", hud_fAlternateCrosshairSpreadNeutral, 6.0f, VF_CHEAT, "", NULL);

GAMECVAR(float, "hud_chDamageIndicator", hud_chDamageIndicator, 1, 0, "Switch crosshair-damage indicator... (1 on, 0 off)", NULL);
GAMECVAR(int, "hud_showAllObjectives", hud_showAllObjectives, 0, 0, "Show all on screen objectives, not only the active one.", NULL);
GAMECVAR(int, "hud_panoramicHeight", hud_panoramicHeight, 10, 0, "Set screen border for 'cinematic view' in percent.", CHUD::OnSubtitlePanoramicHeightCVarChanged);
GAMECVAR(int, "hud_subtitles", hud_subtitles, 0, 0, "Subtitle mode. 0==Off, 1=All, 2=CutscenesOnly", CHUD::OnSubtitleCVarChanged);
GAMECVAR(int, "hud_subtitlesDebug", hud_subtitlesDebug, 0, 0, "Debug subtitles", NULL);
GAMECVAR(int, "hud_subtitlesRenderMode", hud_subtitlesRenderMode, 0, 0, "Subtitle RenderMode. 0==Flash, 1=3DEngine", NULL);
GAMECVAR(int, "hud_subtitlesFontSize", hud_subtitlesFontSize, 16, 0, "FontSize for Subtitles.", NULL);
GAMECVAR(int, "hud_subtitlesHeight", hud_subtitlesHeight, 10, 0, "Height of Subtitles in Percent. Normally same as hud_PanoramicHeight", NULL);
GAMECVAR(int, "hud_subtitlesShowCharName", hud_subtitlesShowCharName, 1, 0, "Show Character talking along with Subtitle", NULL);
GAMECVAR(int, "hud_subtitlesQueueCount", hud_subtitlesQueueCount, 1, 0, "Maximum amount of subtitles in Update Queue", NULL);
GAMECVAR(int, "hud_subtitlesVisibleCount", hud_subtitlesVisibleCount, 1, 0, "Maximum amount of subtitles in Visible Queue", NULL);
GAMECVAR(int, "hud_attachBoughEquipment", hud_attachBoughtEquipment, 0, VF_CHEAT, "Attach equipment in PS equipment packs to the last bought/selected weapon.", NULL);
GAMECVAR(int, "hud_radarBackground", hud_radarBackground, 1, 0, "Switches the miniMap-background for the radar.", NULL);
GAMECVAR(float, "hud_radarJammingEffectScale", hud_radarJammingEffectScale, 0.75f, 0, "Scales the intensity of the radar jamming effect.", NULL);
GAMECVAR(float, "hud_radarJammingThreshold", hud_radarJammingThreshold, 0.99f, 0, "Threshold to disable the radar (independent from effect).", NULL);
GAMECVAR(int, "hud_startPaused", hud_startPaused, 1, 0, "The game starts paused, waiting for user input.", NULL);
GAMECVAR(int, "hud_faderDebug", hud_faderDebug, 0, 0, "Show Debug Information for FullScreen Faders.", NULL);
GAMECVAR(float, "hud_nightVisionConsumption", hud_nightVisionConsumption, 0.5f, VF_CHEAT, "Scales the energy consumption of the night vision.", NULL);
GAMECVAR(float, "hud_nightVisionRecharge", hud_nightVisionRecharge, 2.0f, VF_CHEAT, "Scales the energy recharge of the night vision.", NULL);
GAMECVAR(int, "hud_showBigVehicleReload", hud_showBigVehicleReload, 0, 0, "Enables an additional reload bar around the crosshair in big vehicles.", NULL);
GAMECVAR(float, "hud_radarScanningDelay", hud_binocsScanningDelay, 0.55f, VF_CHEAT, "Defines the delay in seconds the binoculars take to scan an object.", NULL);
GAMECVAR(float, "hud_binocsScanningWidth", hud_binocsScanningWidth, 0.3f, VF_CHEAT, "Defines the width/height in which the binocular raycasts are offset from the center to scan objects.", NULL);

// Controller aim helper cvars
GAMECVAR(float, "aim_assistSearchBox", aim_assistSearchBox, 100.0f, 0, "The area autoaim looks for enemies within", NULL);
GAMECVAR(float, "aim_assistMaxDistance", aim_assistMaxDistance, 150.0f, 0, "The maximum range at which autoaim operates", NULL);
GAMECVAR(float, "aim_assistSnapDistance", aim_assistSnapDistance, 3.0f, 0, "The maximum deviation autoaim is willing to compensate for", NULL);
GAMECVAR(float, "aim_assistVerticalScale", aim_assistVerticalScale, 0.75f, 0, "The amount of emphasis on vertical correction (the less the number is the more vertical component is compensated)", NULL);
GAMECVAR(float, "aim_assistSingleCoeff", aim_assistSingleCoeff, 1.0f, 0, "The scale of single-shot weapons' aim assistance", NULL);
GAMECVAR(float, "aim_assistAutoCoeff", aim_assistAutoCoeff, 0.5f, 0, "The scale of auto weapons' aim assistance at continuous fire", NULL);
GAMECVAR(float, "aim_assistRestrictionTimeout", aim_assistRestrictionTimeout, 20.0f, 0, "The restriction timeout on aim assistance after user uses a mouse", NULL);


// Controller control
GAMECVAR(int, "hud_aspectCorrection", hud_aspectCorrection, 2, 0, "Aspect ratio corrections for controller rotation: 0-off, 1-direct, 2-inverse", NULL);
GAMECVAR(float, "hud_ctrl_Curve_X", hud_ctrl_Curve_X, 3.0f, 0, "Analog controller X rotation curve", NULL);
GAMECVAR(float, "hud_ctrl_Curve_Z", hud_ctrl_Curve_Z, 3.0f, 0, "Analog controller Z rotation curve", NULL);
GAMECVAR(float, "hud_ctrl_Coeff_X", hud_ctrl_Coeff_X, 3.5f * 3.5f, 0, "Analog controller X rotation scale", NULL); // was 3.5*3.5 but aspect ratio correction does the scaling now! adjust only if that gives no satisfactory results
GAMECVAR(float, "hud_ctrl_Coeff_Z", hud_ctrl_Coeff_Z, 5.0f * 5.0f, 0, "Analog controller Z rotation scale", NULL);
GAMECVAR(int, "hud_ctrlZoomMode", hud_ctrlZoomMode, 0, 0, "Weapon aiming mode with controller. 0 is same as mouse zoom, 1 cancels at release", NULL);

GAMECVAR(float, "g_combatFadeTime", g_combatFadeTime, 17.0f, 0, "sets the battle fade time in seconds ", NULL);
GAMECVAR(float, "g_combatFadeTimeDelay", g_combatFadeTimeDelay, 7.0f, 0, "waiting time before the battle starts fading out, in seconds ", NULL);
GAMECVAR(float, "g_battleRange", g_battleRange, 50.0f, 0, "sets the battle range in meters ", NULL);

// Assistance switches
GAMECVAR(int, "aim_assistAimEnabled", aim_assistAimEnabled, 1, 0, "Enable/disable aim assitance on aim zooming", NULL);
GAMECVAR(int, "aim_assistTriggerEnabled", aim_assistTriggerEnabled, 1, 0, "Enable/disable aim assistance on firing the weapon", NULL);
GAMECVAR(int, "hit_assistSingleplayerEnabled", hit_assistSingleplayerEnabled, 1, 0, "Enable/disable minimum damage hit assistance", NULL);
GAMECVAR(int, "hit_assistMultiplayerEnabled", hit_assistMultiplayerEnabled, 1, 0, "Enable/disable minimum damage hit assistance in multiplayer games", NULL);

//movement cvars
GAMECVAR(int, "v_profileMovement", v_profileMovement, 0, 0, "Used to enable profiling of the current vehicle movement (1 to enable)", NULL);
GAMECVAR(int, "v_pa_surface", v_pa_surface, 1, VF_CHEAT, "Enables/disables vehicle surface particles", NULL);
GAMECVAR(float, "v_wind_minspeed", v_wind_minspeed, 0.f, VF_CHEAT, "If non-zero, vehicle wind areas always set wind >= specified value", NULL);
GAMECVAR(int, "v_draw_suspension", v_draw_suspension, 0, VF_DUMPTODISK, "Enables/disables display of wheel suspension, for the vehicle that has v_profileMovement enabled", NULL);
GAMECVAR(int, "v_draw_slip", v_draw_slip, 0, VF_DUMPTODISK, "Draw wheel slip status", NULL);
GAMECVAR(int, "v_invertPitchControl", v_invertPitchControl, 0, VF_DUMPTODISK, "Invert the pitch control for driving some vehicles, including the helicopter and the vtol", NULL);
GAMECVAR(float, "v_sprintSpeed", v_sprintSpeed, 0.f, 0, "Set speed for acceleration measuring", NULL);
GAMECVAR(int, "v_rockBoats", v_rockBoats, 1, 0, "Enable/disable boats idle rocking", NULL);
GAMECVAR(int, "v_dumpFriction", v_dumpFriction, 0, 0, "Dump vehicle friction status", NULL);
GAMECVAR(int, "v_debugSounds", v_debugSounds, 0, 0, "Enable/disable vehicle sound debug drawing", NULL);
GAMECVAR(int, "v_debugMountedWeapon", v_debugMountedWeapon, 0, 0, "Enable/disable vehicle mounted weapon camera debug draw", NULL);
GAMECVAR(int, "v_newBrakingFriction", v_newBrakingFriction, 1, VF_CHEAT, "Change rear wheel friction under handbraking (true/false)", NULL);
GAMECVAR(int, "v_newBoost", v_newBoost, 1, VF_CHEAT, "Apply new boost scheme (true/false)", NULL);

GAMECVAR(int, "v_help_tank_steering", v_help_tank_steering, 0, 0, "Enable tank steering help for AI", NULL);

GAMECVAR(float, "v_stabilizeVTOL", v_stabilizeVTOL, 0.35f, VF_DUMPTODISK, "Specifies if the air movements should automatically stabilize", NULL);


GAMECVAR(float, "pl_swimBaseSpeed", pl_swimBaseSpeed, 4.0f, VF_CHEAT, "Swimming base speed.", NULL);
GAMECVAR(float, "pl_swimBackSpeedMul", pl_swimBackSpeedMul, 0.8f, VF_CHEAT, "Swimming backwards speed mul.", NULL);
GAMECVAR(float, "pl_swimSideSpeedMul", pl_swimSideSpeedMul, 0.9f, VF_CHEAT, "Swimming sideways speed mul.", NULL);
GAMECVAR(float, "pl_swimVertSpeedMul", pl_swimVertSpeedMul, 0.5f, VF_CHEAT, "Swimming vertical speed mul.", NULL);
GAMECVAR(float, "pl_swimNormalSprintSpeedMul", pl_swimNormalSprintSpeedMul, 1.5f, VF_CHEAT, "Swimming Non-Speed sprint speed mul.", NULL);
GAMECVAR(float, "pl_swimSpeedSprintSpeedMul", pl_swimSpeedSprintSpeedMul, 2.5f, VF_CHEAT, "Swimming Speed sprint speed mul.", NULL);
GAMECVAR(float, "pl_swimUpSprintSpeedMul", pl_swimUpSprintSpeedMul, 2.0f, VF_CHEAT, "Swimming sprint while looking up (dolphin rocket).", NULL);
GAMECVAR(float, "pl_swimJumpStrengthCost", pl_swimJumpStrengthCost, 50.0f, VF_CHEAT, "Swimming strength shift+jump energy cost (dolphin rocket).", NULL);
GAMECVAR(float, "pl_swimJumpStrengthSprintMul", pl_swimJumpStrengthSprintMul, 2.5f, VF_CHEAT, "Swimming strength shift+jump velocity mul (dolphin rocket).", NULL);
GAMECVAR(float, "pl_swimJumpStrengthBaseMul", pl_swimJumpStrengthBaseMul, 1.0f, VF_CHEAT, "Swimming strength normal jump velocity mul (dolphin rocket).", NULL);
GAMECVAR(float, "pl_swimJumpSpeedCost", pl_swimJumpSpeedCost, 50.0f, VF_CHEAT, "Swimming speed shift+jump energy cost (dolphin rocket).", NULL);
GAMECVAR(float, "pl_swimJumpSpeedSprintMul", pl_swimJumpSpeedSprintMul, 2.5f, VF_CHEAT, "Swimming speed shift+jump velocity mul (dolphin rocket).", NULL);
GAMECVAR(float, "pl_swimJumpSpeedBaseMul", pl_swimJumpSpeedBaseMul, 1.0f, VF_CHEAT, "Swimming speed normal jump velocity mul (dolphin rocket).", NULL);

GAMECVAR(float, "pl_fallDamage_SpeedSafe", pl_fallDamage_Normal_SpeedSafe, 8.0f, VF_CHEAT, "Safe fall speed (in all modes, including strength jump on flat ground).", NULL);
GAMECVAR(float, "pl_fallDamage_SpeedFatal", pl_fallDamage_Normal_SpeedFatal, 13.7f, VF_CHEAT, "Fatal fall speed in armor mode (13.5 m/s after falling freely for ca 20m).", NULL);
GAMECVAR(float, "pl_fallDamage_SpeedBias", pl_fallDamage_SpeedBias, 1.5f, VF_CHEAT, "Damage bias for medium fall speed: =1 linear, <1 more damage, >1 less damage.", NULL);
GAMECVAR(int, "pl_debugFallDamage", pl_debugFallDamage, 0, VF_CHEAT, "Enables console output of fall damage information.", NULL);


GAMECVAR(float, "pl_zeroGSpeedMultNormal", pl_zeroGSpeedMultNormal, 1.2f, VF_CHEAT, "Modify movement speed in zeroG, in normal mode.", NULL);
GAMECVAR(float, "pl_zeroGSpeedMultNormalSprint", pl_zeroGSpeedMultNormalSprint, 1.7f, VF_CHEAT, "Modify movement speed in zeroG, in normal sprint.", NULL);
GAMECVAR(float, "pl_zeroGSpeedMultSpeed", pl_zeroGSpeedMultSpeed, 1.7f, VF_CHEAT, "Modify movement speed in zeroG, in speed mode.", NULL);
GAMECVAR(float, "pl_zeroGSpeedMultSpeedSprint", pl_zeroGSpeedMultSpeedSprint, 5.0f, VF_CHEAT, "Modify movement speed in zeroG, in speed sprint.", NULL);
GAMECVAR(float, "pl_zeroGUpDown", pl_zeroGUpDown, 1.0f, 0, "Scales the z-axis movement speed in zeroG.", NULL);
GAMECVAR(float, "pl_zeroGBaseSpeed", pl_zeroGBaseSpeed, 3.0f, 0, "Maximum player speed request limit for zeroG.", NULL);
GAMECVAR(float, "pl_zeroGSpeedMaxSpeed", pl_zeroGSpeedMaxSpeed, -1.0f, 0, "(DEPRECATED) Maximum player speed request limit for zeroG while in speed mode.", NULL);
GAMECVAR(float, "pl_zeroGSpeedModeEnergyConsumption", pl_zeroGSpeedModeEnergyConsumption, 0.5f, 0, "Percentage consumed per second while speed sprinting in ZeroG.", NULL);
GAMECVAR(float, "pl_zeroGDashEnergyConsumption", pl_zeroGDashEnergyConsumption, 0.25f, 0, "Percentage consumed when doing a dash in ZeroG.", NULL);
GAMECVAR(int, "pl_zeroGSwitchableGyro", pl_zeroGSwitchableGyro, 0, 0, "MERGE/REVERT", NULL);
GAMECVAR(int, "pl_zeroGEnableGBoots", pl_zeroGEnableGBoots, 0, 0, "Switch G-Boots action on/off (if button assigned).", NULL);
GAMECVAR(float, "pl_zeroGThrusterResponsiveness", pl_zeroGThrusterResponsiveness, 0.3f, VF_CHEAT, "Thrusting responsiveness.", NULL);
GAMECVAR(float, "pl_zeroGFloatDuration", pl_zeroGFloatDuration, 1.25f, VF_CHEAT, "Floating duration until full stop (after stopped thrusting).", NULL);
GAMECVAR(int, "pl_zeroGParticleTrail", pl_zeroGParticleTrail, 0, 0, "Enable particle trail when in zerog.", NULL);
GAMECVAR(int, "pl_zeroGEnableGyroFade", pl_zeroGEnableGyroFade, 2, VF_CHEAT, "Enable fadeout of gyro-stabilizer for vertical view angles (2=disable speed fade as well).", NULL);
GAMECVAR(float, "pl_zeroGGyroFadeAngleInner", pl_zeroGGyroFadeAngleInner, 20.0f, VF_CHEAT, "ZeroG gyro inner angle (default is 20).", NULL);
GAMECVAR(float, "pl_zeroGGyroFadeAngleOuter", pl_zeroGGyroFadeAngleOuter, 60.0f, VF_CHEAT, "ZeroG gyro outer angle (default is 60).", NULL);
GAMECVAR(float, "pl_zeroGGyroFadeExp", pl_zeroGGyroFadeExp, 2.0f, VF_CHEAT, "ZeroG gyro angle bias (default is 2.0).", NULL);
GAMECVAR(float, "pl_zeroGGyroStrength", pl_zeroGGyroStrength, 1.0f, VF_CHEAT, "ZeroG gyro strength (default is 1.0).", NULL);
GAMECVAR(float, "pl_zeroGAimResponsiveness", pl_zeroGAimResponsiveness, 8.0f, VF_CHEAT, "ZeroG aim responsiveness vs. inertia (default is 8.0).", NULL);

GAMECVAR(float, "tracer_min_distance", tracer_min_distance, 4.0f, 0, "Distance at which to start scaling/lengthening tracers.", NULL);
GAMECVAR(float, "tracer_max_distance", tracer_max_distance, 50.0f, 0, "Distance at which to stop scaling/lengthening tracers.", NULL);
GAMECVAR(float, "tracer_min_scale", tracer_min_scale, 0.5f, 0, "Scale at min distance.", NULL);
GAMECVAR(float, "tracer_max_scale", tracer_max_scale, 5.0f, 0, "Scale at max distance.", NULL);
GAMECVAR(int, "tracer_max_count", tracer_max_count, 32, 0, "Max number of active tracers.", NULL);
GAMECVAR(float, "tracer_player_radiusSqr", tracer_player_radiusSqr, 400.0f, 0, "Sqr Distance around player at which to start decelerate/acelerate tracer speed.", NULL);

GAMECVAR(int, "i_debug_projectiles", i_debug_projectiles, 0, VF_CHEAT, "Displays info about projectile status, where available.", NULL);
GAMECVAR(int, "i_auto_turret_target", i_auto_turret_target, 1, VF_CHEAT, "Enables/Disables auto turrets aquiring targets.", NULL);
GAMECVAR(int, "i_auto_turret_target_tacshells", i_auto_turret_target_tacshells, 0, 0, "Enables/Disables auto turrets aquiring TAC shells as targets", NULL);

GAMECVAR(int, "i_debug_zoom_mods", i_debug_zoom_mods, 0, VF_CHEAT, "Use zoom mode spread/recoil mods", NULL);
GAMECVAR(int, "i_debug_sounds", i_debug_sounds, 0, VF_CHEAT, "Enable item sound debugging", NULL);
GAMECVAR(int, "i_debug_turrets", i_debug_turrets, 0, VF_CHEAT,
	"Enable GunTurret debugging.\n"
	"Values:\n"
	"0:  off"
	"1:  basics\n"
	"2:  prediction\n"
	"3:  sweeping\n"
	"4:  searching\n"
	"5:  deviation\n",
	NULL);
GAMECVAR(int, "i_debug_mp_flowgraph", i_debug_mp_flowgraph, 0, VF_CHEAT, "Displays info on the MP flowgraph node", NULL);

GAMECVAR(float, "h_turnSpeed", h_turnSpeed, 1.3f, 0, "", NULL);

// quick game
GAMECVAR(int, "g_quickGame_min_players", g_quickGame_min_players, 0, VF_DUMPTODISK, "QuickGame option", NULL);
GAMECVAR(int, "g_quickGame_prefer_lan", g_quickGame_prefer_lan, 0, VF_DUMPTODISK, "QuickGame option", NULL);
GAMECVAR(int, "g_quickGame_prefer_favorites", g_quickGame_prefer_favorites, 0, VF_DUMPTODISK, "QuickGame option", NULL);
GAMECVAR(int, "g_quickGame_prefer_mycountry", g_quickGame_prefer_my_country, 0, VF_DUMPTODISK, "QuickGame option", NULL);
GAMECVAR(int, "g_quickGame_ping1_level", g_quickGame_ping1_level, 80, VF_DUMPTODISK, "QuickGame option", NULL);
GAMECVAR(int, "g_quickGame_ping2_level", g_quickGame_ping2_level, 170, VF_DUMPTODISK, "QuickGame option", NULL);

GAMECVAR(int, "g_quickGame_debug", g_quickGame_debug, 0, VF_CHEAT, "QuickGame option", NULL);

GAMECVAR(int, "g_displayIgnoreList", g_displayIgnoreList, 1, VF_DUMPTODISK, "Display ignore list in chat tab.", NULL);
GAMECVAR(int, "g_buddyMessagesIngame", g_buddyMessagesIngame, 1, VF_DUMPTODISK, "Output incoming buddy messages in chat while playing game.", NULL);

// battledust
GAMECVAR(int, "g_battleDust_enable", g_battleDust_enable, 1, 0, "Enable/Disable battledust", NULL);
GAMECVAR(int, "g_battleDust_debug", g_battleDust_debug, 0, 0, "0: off, 1: text, 2: text+gfx", NULL);

GAMECVAR(int, "g_PSTutorial_Enabled", g_PSTutorial_Enabled, 1, 0, "Enable/disable powerstruggle tutorial", NULL);

GAMECVAR(int, "g_proneNotUsableWeapon_FixType", g_proneNotUsableWeapon_FixType, 1, 0, "Test various fixes for not selecting hurricane while prone", NULL);
GAMECVAR(int, "g_proneAimAngleRestrict_Enable", g_proneAimAngleRestrict_Enable, 1, 0, "Test fix for matching aim restrictions between 1st and 3rd person", NULL);

GAMECVAR(int, "sv_voting_timeout", sv_votingTimeout, 60, 0, "Voting timeout", NULL);
GAMECVAR(int, "sv_voting_cooldown", sv_votingCooldown, 180, 0, "Voting cooldown", NULL);
GAMECVAR(float, "sv_voting_ratio", sv_votingRatio, 0.51f, 0, "Part of player's votes needed for successful vote.", NULL);
GAMECVAR(float, "sv_voting_team_ratio", sv_votingTeamRatio, 0.67f, 0, "Part of team member's votes needed for successful vote.", NULL);

GAMECVAR(int, "sv_input_timeout", sv_input_timeout, 0, 0, "Experimental timeout in ms to stop interpolating client inputs since last update.", NULL);

GAMECVAR(int, "g_spectate_TeamOnly", g_spectate_TeamOnly, 1, 0, "If true, you can only spectate players on your team", NULL);
GAMECVAR(int, "g_spectate_FixedOrientation", g_spectate_FixedOrientation, 0, 0, "If true, spectator camera is fixed behind player. Otherwise spectator controls orientation", NULL);
GAMECVAR(int, "g_claymore_limit", g_claymore_limit, 3, 0, "Max claymores a player can place (recycled above this value)", NULL);
GAMECVAR(int, "g_avmine_limit", g_avmine_limit, 3, 0, "Max avmines a player can place (recycled above this value)", NULL);
GAMECVAR(int, "g_debugMines", g_debugMines, 0, 0, "Enable debug output for mines and claymores", NULL);

GAMECVAR(int, "aim_assistCrosshairSize", aim_assistCrosshairSize, 25, VF_CHEAT, "screen size used for crosshair aim assistance", NULL);
GAMECVAR(int, "aim_assistCrosshairDebug", aim_assistCrosshairDebug, 0, VF_CHEAT, "debug crosshair aim assistance", NULL);

GAMECVAR(int, "g_MPDeathCam", g_deathCam, 1, 0, "Enables / disables the MP death camera (shows the killer's location)", NULL);
GAMECVAR(int, "g_MPDeathEffects", g_deathEffects, 0, 0, "Enables / disables the MP death screen-effects", NULL);

GAMECVAR(int, "sv_pacifist", sv_pacifist, 0, 0, "Pacifist mode (only works on dedicated server)", NULL);

//CryMP CVars 
GAMECVAR(int, "mp_crymp", mp_crymp, 0, OPTIONAL_SYNC, "Enable high precision look direction serialization", NULL);
GAMECVAR(float, "mp_circleJump", mp_circleJump, 0.0f, OPTIONAL_SYNC, "Enable circle jumping as in 5767", NULL);
GAMECVAR(float, "mp_wallJump", mp_wallJump, 1.0f, OPTIONAL_SYNC, "WallJump multiplier", NULL);
GAMECVAR(int, "mp_flyMode", mp_flyMode, 0, OPTIONAL_SYNC, "Enable FlyMode", OnChangeFlyMode);
GAMECVAR(int, "mp_pickupObjects", mp_pickupObjects, 0, OPTIONAL_SYNC, "Allow pickup and throw objects in DX10", NULL);
GAMECVAR(int, "mp_pickupVehicles", mp_pickupVehicles, 0, OPTIONAL_SYNC, "Allow pickup and throw vehicles (requires mp_pickupObjects 1)", NULL);
GAMECVAR(int, "mp_weaponsOnBack", mp_weaponsOnBack, 0, OPTIONAL_SYNC, "Attach weapons to back as in SP", NULL);
GAMECVAR(int, "mp_thirdPerson", mp_thirdPerson, 1, OPTIONAL_SYNC, "Allow ThirdPerson mode (F1)", OnChangeThirdPerson);
GAMECVAR(int, "mp_animationGrenadeSwitch", mp_animationGrenadeSwitch, 0, OPTIONAL_SYNC, "Enable FP animations for grenade switching", NULL);
GAMECVAR(int, "mp_ragdollUnrestricted", mp_ragdollUnrestricted, 1, OPTIONAL_SYNC, "", NULL);
GAMECVAR(int, "mp_killMessages", mp_killMessages, 1, OPTIONAL_SYNC, "", NULL);
GAMECVAR(int, "mp_rpgMod", mp_rpgMod, 0, OPTIONAL_SYNC, "", NULL);
GAMECVAR(int, "mp_aaLockOn", mp_aaLockOn, 0, OPTIONAL_SYNC, "enables lockon air for AARocketLauncher", NULL);
GAMECVAR(float, "mp_C4StrengthThrowMult", mp_C4StrengthThrowMult, 1.0f, OPTIONAL_SYNC, "Strength throw mult for C4s", NULL);

//CryMP CVars (un-synced)
GAMECVAR(int, "mp_newSpectator", mp_newSpectator, 1, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(int, "mp_usePostProcessAimDir", mp_usePostProcessAimDir, 1, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(int, "mp_messageCenterColor", mp_messageCenterColor, 1, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(float, "mp_animationWeaponMult", mp_animationWeaponMult, 1.5f, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(float, "mp_animationWeaponMultSpeed", mp_animationWeaponMultSpeed, 3.0f, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(float, "mp_animationModelMult", mp_animationModelMult, 1.0f, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(float, "mp_animationModelMultSpeed", mp_animationModelMultSpeed, 1.0f, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(float, "mp_menuSpeed", mp_menuSpeed, 3.0f, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(int, "mp_hitIndicator", mp_hitIndicator, 1, VF_NOT_NET_SYNCED, "Enables hit indicator from Wars", NULL);
GAMECVAR(int, "mp_chatHighResolution", mp_chatHighResolution, 0, VF_NOT_NET_SYNCED, "", NULL);
GAMECVAR(float, "mp_spectatorSlowMult", mp_spectatorSlowMult, 0.15f, VF_NOT_NET_SYNCED, "Speed mult for spectating while holding Ctrl", NULL);
GAMECVAR(int, "mp_buyPageKeepTime", mp_buyPageKeepTime, 30, VF_NOT_NET_SYNCED, "The time in sec it will remember your last buy page", NULL);
GAMECVAR(int, "mp_attachBoughtEquipment", mp_attachBoughtEquipment, 0, VF_NOT_NET_SYNCED, "Automatically attach bought weapon attachments", NULL);
GAMECVAR(float, "mp_netAimLerpFactor", mp_netAimLerpFactor, 20.f, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "set aim smoothing for other clients (1-50, 0:off)", NULL);
GAMECVAR(float, "mp_netAimLerpFactorCrymp", mp_netAimLerpFactorCrymp, 42.f, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "set aim smoothing for other clients when mp_crymp 1 (1-50, 0:off)", NULL);
GAMECVAR(int, "mp_explosiveSilhouettes", mp_explosiveSilhouettes, 0, VF_NOT_NET_SYNCED/*VF_CHEAT*/, "enables new indicators for explosives", NULL);


#undef GAMECVAR

#endif //__GAMECVARSLIB_H__
