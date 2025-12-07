# Axe Dodge (C++ + raylib)

A 2D horror runner game developed in C++ with Raylib.
Includes custom player physics, dynamic background effects, obstacle system, collision detection, and looping background music. Designed as a learning project focusing on game mechanics, animation, and real-time input handling.

## Demo
_Add a screenshot or GIF here (e.g., <img width="347" height="216" alt="Capture d’écran 2025-12-07 à 21 11 32" src="https://github.com/user-attachments/assets/34aec058-81f1-4662-bdf9-cd26bd360610" />
`docs/screenshot.png`)._

## Features
Controls are simple: move left and right with A/D or the arrow keys, jump with Space, W, or Up, and restart the game with R. The project uses a clean C++ codebase split into multiple files (main game loop and player handling), along with a Makefile for building. All required assets—Player.png, Theme.wav, and GameOver.wav—should be placed in the same folder as the executable.

## Build & Run (Windows, w64devkit + raylib)
1. Install the official raylib Windows kit so you have `C:\raylib\w64devkit` and headers/libs.
2. From this project folder:
   ```bat
   C:\raylib\w64devkit\bin\mingw32-make.exe
   .\axe_game.exe
