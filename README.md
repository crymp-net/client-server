# CryMP

Improved [Crysis (2007)](https://en.wikipedia.org/wiki/Crysis_(video_game)) with [CryMP](https://crymp.net) community
multiplayer. Singleplayer is supported as well.

The official multiplayer no longer works due to [GameSpy shutdown](https://en.wikipedia.org/wiki/GameSpy#Shutdown).
Now it is our turn to keep the multiplayer running and make it even better than before.

Features of [C1-Launcher](https://github.com/ccomrade/c1-launcher) are included.

Join us at [Discord](https://discord.gg/kY8hb2n).

## Goals

- Provide multiplayer services.
- Continue improving the game to keep it alive.
- Reverse engineer and reimplement the whole game and its engine to have a complete source code.

## Limitations

- Existing Crysis mods that have their own DLLs cannot work with this.

## Installation

### Step 1: Install Crysis (2007)

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 5767  | Y      | Y      | see below |
| [GOG](https://www.gog.com/game/crysis)                            | 1.2.1   | 6156  | Y      | Y      | Y         |
| [Steam](https://store.steampowered.com/app/17300/Crysis/)         | 1.2.1   | 6156  | Y      | Y      | Y         |
| [EA](https://www.ea.com/games/crysis/crysis)                      | 1.2.1   | 6156  | Y      | Y      | Y         |

The latest official version 1.2.1 (6156) is needed. If you have Crysis from DVD, install
[1.2](https://comrade.one/Crysis_Patch_1_2.exe) and
[1.2.1](https://comrade.one/Crysis_Patch_1_2_1.exe) official patches.

### Step 2: Install CryMP

#### Option A: Using the launcher (recommended)

- Go to [CryMP](https://crymp.net) website and get the launcher by clicking on Play now.
- Run the launcher and click on Install. If the Install button is disabled, first click on the Crysis directory path
and choose where Crysis is installed. The launcher will copy itself there, download the latest CryMP client and create
a desktop shortcut called Crysis Multiplayer.
- You can now click on Play to run the game. To open the launcher again, use the desktop shortcut or double-click on
`CryMP-Launcher.exe` inside Crysis directory.

The launcher allows you to easily log-in to your CryMP account (optional) and takes care of updating the CryMP client
when a new version is released.

Source code of the launcher is [here](https://github.com/crymp-net/launcher).

#### Option B: Manual installation

- Get the latest [release](https://github.com/crymp-net/client-server/releases) or build the code in this repository.
- Copy `CryMP-Client32.exe` (32-bit version) and/or `CryMP-Client64.exe` (64-bit version) into Crysis directory.
- You can now run the game by double-clicking on one of the EXEs.
