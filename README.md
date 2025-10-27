# Axe Dodge (C++ + raylib)

A tiny arcade game: move the blue circle with **A/D** and dodge the red axe that bounces vertically.  
You gain **+1 score per second** survived. The axe **speeds up every 5 points**.  
Press **R** after collision to **restart**. Includes simple **sound effects**.

## Demo
_Add a screenshot or GIF here (e.g., `docs/screenshot.png`)._

## Features
- Real-time game loop with raylib
- Keyboard input (A/D, R)
- AABB collision detection
- Frame-rate independent timing (`GetFrameTime`)
- Difficulty ramp (axe speed increases with score)
- Sound effects (`point.wav` on score, `GameOver.wav` on hit)
- VS Code tasks/launch + Makefile

## Build & Run (Windows, w64devkit + raylib)
1. Install the official raylib Windows kit so you have `C:\raylib\w64devkit` and headers/libs.
2. From this project folder:
   ```bat
   C:\raylib\w64devkit\bin\mingw32-make.exe
   .\axe_game.exe
