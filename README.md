# Wallpaper Shortcut

A lightweight Windows utility that opens File Explorer with the currently
displayed desktop wallpaper selected.

## Features
- Retrieves the current Windows desktop wallpaper
- Supports multiple monitors
- Filters out detached displays
- Prompts for a monitor when multiple displays are active
- Opens File Explorer with the wallpaper file selected
- Distributed as a standalone Windows executable

## Usage
1. Run `wallpaper-shortcut.exe`.
2. If one display is active, the current wallpaper opens immediately in Explorer.
3. If multiple displays are active, select a monitor.
4. Explorer opens with that monitor's wallpaper selected.

## How It Works
Uses the Windows `IDesktopWallpaper` COM interface to enumerate displays and
retrieve the wallpaper assigned to each active monitor. The selected file is
then opened in File Explorer using the Windows Shell API.

## Building From Source

### Requirements
- Windows
- MSYS2 UCRT64
- GCC / G++
- `windres`

### Build
```bash
windres resource.rc -o resource.o
g++ main.cpp resource.o -o wallpaper-shortcut.exe -static -mwindows -lole32 -luuid -lshell32

[Click to Download](https://github.com/cheungxucheng/wallpaper-shortcut/releases/download/win32/Wallpaper.Shortcut.exe)
