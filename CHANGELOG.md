# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]
- [Fix damage indicator not showing when laser enabled](https://github.com/crymp-net/client-server/pull/119)
- [Remove TypeInfo](https://github.com/crymp-net/client-server/pull/120)
- [Fix projectile lag on spawn](https://github.com/crymp-net/client-server/pull/121)
- [Fix double click bug in menu on certain resolutions](https://github.com/crymp-net/client-server/pull/122)
- [Fix WinHTTP error messages](https://github.com/crymp-net/client-server/pull/123)
- [Add repair smoothing](https://github.com/crymp-net/client-server/pull/124)
- Fix some minor vehicle sound issues
- [Fix out-of-bounds access of the CPUInfo::cores array](https://github.com/crymp-net/client-server/pull/125)
- [Fix CPUID::TrimSpaces](https://github.com/crymp-net/client-server/pull/126)
- [Fix movement bug occuring after unfreeze](https://github.com/crymp-net/client-server/pull/127)
- Fix shadow direction in first person
- [Fix severity of unimportant warnings in Script System](https://github.com/crymp-net/client-server/pull/128)

## [v20] - 2023-11-11
### Added
- New third-person camera inspired by Hitman
- Third-person view on ladder
- Reverse view in unarmed vehicles by holding R key
- Automatic attaching of bought accessories to current weapon (`mp_attachBoughtEquipment`)
- Console server list from the old multiplayer client (type `0` in console)
- Server controllable RPGMod (`mp_rpgMod`)
- Server controllable AA missile lock-on (`mp_aaLockOn`)
- In-game chat clipboard paste (Ctrl + V)
- In-game chat history (up and down arrows)
- Workaround for missing localization files in Crysis from Steam
- `-language` command line parameter to specify game language
- C4 throw sound and `mp_C4StrengthThrowMult`
- New kill-log icons
- New parachute animation and sounds
- `reconnect` console command
- GPU information in the log:
```
Renderer initialization
D3D10 Adapter: AMD Radeon RX 480 Graphics (RADV POLARIS10)
D3D10 Adapter: PCI 1002:67df (rev 00)
D3D10 Adapter: Dedicated video memory = 8G
D3D10 Adapter: Dedicated system memory = 0
D3D10 Adapter: Shared system memory = 31G
```
- New Lua functions:
    - `CPPAPI.DrawText`
    - `CPPAPI.DrawImage`
    - `CPPAPI.DrawColorBox`
    - `CPPAPI.RemoveTextOrImageById`
    - `CPPAPI.RemoveTextOrImageAll`
    - `CPPAPI.GetLastSeenTime`
    - `CPPAPI.GetLP`
    - `CPPAPI.GetNumVars`
    - `CPPAPI.GetLoadingScreenMapPicturePath`
    - `CPPAPI.VehicleNoSeatChangeAndExit`
    - `HUD.SetSilhouette`
    - `HUD.ResetSilhouette`
    - `Sound.GetParameterValue` and improved `Sound.SetParameterValue`

### Changed
- [Improved project version](../../pull/94)
- [Improved buy-menu responsiveness](../../pull/106)
- Improved vehicle damage sounds
- [Improved script system](../../pull/109)
- [Improved no connection detection in multiplayer lobby](../../pull/111)
- Improved third-person and spectator camera in freefall
- Improved repair-kit and lockpick-kit
- Improved anti-cheat
- Key bindings created before joining a game are no longer removed upon disconnect
- Adjusted aim smoothing (`mp_netAimLerpFactorCrymp`)
- Team-colored weapon lock-on silhouettes
- Smooth capture icon animation in power struggle
- Show prestige points and battle-log while dead
- Allow spectating of non-actor entities
- Dev builds are now signed as well

### Fixed
- Various issues related to showing explosives
- Various potential RMI issues
- Various vehicle and parachute issues
- Various weapon lock-on issues, thanks to Furyaner
- Ladder glitch
- Mouse actions in spectator
- Connecting to local servers via internet lobby
- Laser stuck in air when buying RPG while dead
- Visible model on shatter
- Binoculars in first-person spectator
- Shiten transition to third-person, missing legs
- Shotgun animation playing too fast
- [NPC flashlights after loading a saved game](../../pull/98)
- Missing AA tracer
- Lua `io` table removal
- Crosshair dissapearing in MP due to delta difficulty in SP
- Crash of 64-bit DX10 renderer during startup with recent nVidia GPU drivers
- Spectators interfering with players

## [v19] - 2022-12-18
### Fixed
- Shooting while sprinting and some related issues

## [v18] - 2022-12-18
### Added
- Hit indicator, enabled by default, use `mp_hitIndicator 0` to disable it
- High resolution chat HUD, disabled by default, use `mp_chatHighResolution 1` to enable it
- New spectator HUD
- New radio HUD allowing servers to enable additional commands
- New localization manager
- New CPU detection
- New Lua function for creating a material from any texture in the game, for example:
```lua
CPPAPI.CreateMaterialFromTexture("MyNewMaterial", "Libs/UI/HUD_PDA_Map_FLAG_NK.dds")
```
- New Lua function for changing entity opacity, for example:
```lua
CPPAPI.SetOpacity(entity.id, 0.2)
```
- New Lua function for obtaining current language, for example:
```lua
local language = CPPAPI.GetLanguage()
```
- New Lua function for text localization, for example:
```lua
local message = CPPAPI.LocalizeText("Kyong says: @mine/kyong_mine_ab3_A584F343")
```
- New Lua function for adding localized labels, see [Scripts/Localization.lua](Scripts/Localization.lua)
- New Lua global variable `CRYMP_CLIENT_BITS`
- Archetypes synchronized over network, see `Client::OnBeforeSpawn`
- New application icon
- Spectator zooming
- Slow spectator movement when holding Ctrl, speed can be configured by changing `mp_spectatorSlowMult`

### Changed
- Improved map loading screen
- Pressing F5 in multiplayer lobby refreshes the server list
- Two `!validate` commands are now sent after connect to improve backward compatibility with the old client
- Internal chat commands `!validate` and `!rpc` are hidden in chat to improve UX on legacy servers
- Buymenu remembers your last opened page for some time, see `mp_buyPageKeepTime`
- Battlelog messsages can now be longer
- Improved logger
- Improved crash logger
- Better game window name
- [Map downloader now supports map versioning](../../issues/53)
- Released executables are now digitally signed

### Fixed
- DX10 fullscreen low refresh rate fix now works correctly
- Missing US commander mission end warning in PowerStruggle
- Missing `@detonator` localized label
- Team selection HUD still visible after joining a team
- Sensitibity bug after dying with picked up object
- No more vehicle pickup while in air
- Some crashes caused by uninitialized memory accesses in the engine
- Dead projectiles
- No more crosshair for ragdolls in FP spec
- Case-sensitive command line options

## [v17] - 2022-08-28
### Fixed
- Temporarily disabled loading of CryRenderD3D10 DLL causing crash on startup

## [v16] - 2022-08-28
### Added
- New multiplayer map loading screen
- New buy screen in PDA HUD
- New vehicle statistics HUD
- Improved chat HUD from Crysis Wars

### Changed
- CD key is now random to avoid kicks from legacy servers. Can be disabled with `-keepcdkey`.
- Master server domain changed from `crymp.net` to `crymp.nullptr.one`, which is more reliable.

### Fixed
- Sporadic file check crash is now fixed in both 32-bit and 64-bit version.
- DX10 fullscreen low refresh rate bug
- Nonexistent weapons on back
- Attempt to fix sporadic super low sensivity after picking up heavy objects

## [v15] - 2022-07-18
### Changed
- Using the new script system by default, you can still switch to the old one using `-oldss` command line parameter
- Kill log now shows RunOver icon when killing players with object
- Improve First Person spectator vehicle view
- Remove transparency for enemy in radar
- All vehicles can be tagged by binoculars now

### Fixed
- Crashes occuring with Aliens
- Crosshair disappearing in Third Person
- Randomly closing binoculars
- Sporadic file check crash (in 64-bit version only)

### Added
- Scanning objects with binoculars highlights team color (Neutral = white, Team = blue, Enemy = red)
- Spawn info for level loading message
- Draw killer tag also when inside a vehicle

## [v14] - 2022-06-06
### Changed
- Scanning teammates are now highlighted on radar with a green triangle.
- Multiplayer lobby stays open after disconnecting from a server.
- Player view distance is now explicitly set to unlimited again, hopefully fixing the still occurring ghost bug.
- Reworked Third Person attachment menu.

### Fixed
- Missing HP/Energy bar when spectating and then spawning in a spawn truck.
- Minor issues with game window.
- Object and grenade throw in FP spectator.
- Optimized Tornado
- Hunter Sound script error (new ScriptSystem)

## [v13] - 2022-05-22
### Added
- Server PAK feature.
- New internet server browser backend using the CryMP HTTP API. Replaces the GameSpy stuff.
- Map downloader. Maps are stored in the user folder to avoid write permission issues.
- CryMP profiles.
- RPC from the old multiplayer client.
- Multiplayer object pickup and throw. Requires compatible SSM to be activated. See `mp_pickup*` cvars.
- First person spectator mode.
- `mp_circleJump` server-side cvar to enable the old circle jump glitch from 5767. Disabled by default.
- `mp_wallJump` server-side cvar to modify the wall jump glitch. The default value `1.0` means no modification.
- `mp_flyMode` server-side cvar to allow fly mode activation without DEVMODE. Disabled by default.
- Colored kill log in IA.
- Connection state info text.
- `log_Prefix` client-side cvar. See `log_Prefix ?`.
- Optional `-logprefix` command line parameter to specify default value of `log_Prefix`.
- Optional `-verbosity` command line parameter to specify default log verbosity.
- New log implementation compatible with the old one.
- `mp_crymp` server-side cvar allowing server to enable new features incompatible with vanilla clients.
- `cursor_increment` and `cursor_decrement` console commands to change mouse cursor visibility.
- `say` console command useful mainly for binding chat commands to keys.
- Optional `-dir` command line parameter to specify Crysis main directory. It allows the EXE to be placed anywhere.
- Features from [c1-launcher](https://github.com/ccomrade/c1-launcher).
- `mp_menuSpeed` client-side cvar for changing speed of menu animations.
- `mp_weaponsOnBack` server-side cvar for activating the weapons-on-back feature from singleplayer.
- `mp_animationGrenadeSwitch` server-side cvar for activating the first person grenade switch animation.
- `mp_killMessages` server-side cvar for disabling the kill log.

### Changed
- Ranked servers are now marked with `^`.
- Unreachable servers are now grayed out.
- The log file is now stored in the user folder as `CryMP-Client.log` by default.
- Unlocked physical interactions with ragdolls. Useful mainly in singleplayer. See the `mp_ragdollUnrestricted` cvar.
- Binoculars now tag vehicles with drivers.
- Show radar effect when nearby teammates are scanning and highlight them on radar.
- Show friendly C4.
- Show killer name tag always.
- Allow some actions during network lag (chat, look around, modify weapon, zoom).
- Enable weapon raise near doors to fix weapon/laser glitching through them.
- Show player health/energy/suit/weapon info in spectator mode.
- Show battle and kill log in spectator mode.
- Show kick and ban reason.
- Send recoil and zoom sway changes to server.
- Default value of `con_restricted` is `0` instead of `1`.
- Default log verbosity is `0` instead of `1`.
- Multiple game instances cannot use the same log file anymore. Use `-logfile` or `-root` to change the log file.
- No annoying color codes (`$0` .. `$9`) in the log file anymore.
- Log messages from other threads are not being silently dropped anymore.
- All log messages can be disabled by setting `log_Verbosity` or `log_FileVerbosity` to `-1`.
- Optional high precision look direction serialization for the first person spectator mode. Enabled via `mp_crymp 1`.
- No automatic switch to menu when game window loses focus.
- Third person view can be used without DEVMODE. Enabled via `mp_thirdPerson 1`.
- The CryGame DLL is not being used anymore.

### Fixed
- Lean glitches.
- Scope glitches.
- Missing bunker attack info.
- Some game crashes.
- Some minor player animation issues.
- Some third person mode issues.
- Some rail glitches.
- Ghost bug.
- Disappearing enemies on radar during continuous scanning.
- AA tank low FPS bug.
- StopFire RMI flood bug.
- Mouse cursor visibility bugs.

### Deprecated
- `cl_master` client-side cvar with GameSpy master server hostname.

### Removed
- Useless "times out" log messages.
- Useless client-side chat log. It doesn't log chat messages from other players anyway.
- Some GameSpy stuff (natneg, console commands, profile, server browser).
- `log_IncludeTime` cvar. Use the new `log_Prefix` cvar instead.
