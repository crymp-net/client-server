# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

[Unreleased]

### Changed
- Using the new script system by default
- Killlog now shows RunOver icon when killing players with object
- Improve First Person spectator vehicle view
- Remove transparency for enemy in radar
- All vehicles can be tagged by binoculars now

### Fixed
- Crashes occuring with Aliens
- Crosshair disappearing in Third Person
- Fixed Binoculars closing randomly

### Added
- Scanning objects with binoculars highlights team color (Neutral=white,Team=blue,Enemy=red)
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
