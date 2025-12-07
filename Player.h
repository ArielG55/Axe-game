#pragma once
#include "raylib.h"

struct Player
{
    // Position and movement
    float x, y;
    float vx, vy;
    float width, height;
    bool  onGround;

    // Scale + animation time
    float scale;
    float animTime;   // accumulated time, used for the “breathing” motion

    // Single player texture
    Texture2D texture;
};

// API
void InitPlayer(Player &p, float groundY);
void UpdatePlayer(Player &p, float dt, float groundY);
void DrawPlayer(const Player &p);
void UnloadPlayer(Player &p);