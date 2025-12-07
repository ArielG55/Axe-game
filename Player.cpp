#include "Player.h"
#include <cmath>

// Constants – you can tweak these values
static const float GRAVITY       = 1200.0f;
static const float JUMP_SPEED    = -600.0f;
static const float MOVE_ACCEL    = 2600.0f;
static const float MOVE_FRICTION = 2600.0f;
static const float MAX_SPEED     = 320.0f;

// How much to shift the character downward so the feet touch the ground visually
static const float FEET_OFFSET   = 35.0f;   // if he's floating, reduce to 15–20

// ---------------------------------------------------------------
//  InitPlayer : loads the PNG and positions the player
// ---------------------------------------------------------------
void InitPlayer(Player &p, float groundY)
{
    // Load the Player.png file (must be in the same folder as the .exe)
    p.texture = LoadTexture("Player.png");

    // How large/small to draw on screen
    p.scale  = 0.10f;           // if too big, reduce to 0.18/0.16

    p.width  = p.texture.width  * p.scale;
    p.height = p.texture.height * p.scale;

    // Starting position
    p.x = 80.0f;
    p.y = groundY - p.height;   // logically touching the ground
    p.vx = 0.0f;
    p.vy = 0.0f;
    p.onGround = true;

    p.animTime = 0.0f;
}

// ---------------------------------------------------------------
//  UpdatePlayer : physics + input + “breathing” animation
// ---------------------------------------------------------------
void UpdatePlayer(Player &p, float dt, float groundY)
{
    // -------- Left / Right input --------
    float moveDir = 0.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveDir += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  moveDir -= 1.0f;

    if (moveDir != 0.0f)
    {
        p.vx += moveDir * MOVE_ACCEL * dt;
    }
    else
    {
        // Friction – slows the player down when not pressing keys
        if (p.vx > 0.0f)
        {
            p.vx -= MOVE_FRICTION * dt;
            if (p.vx < 0.0f) p.vx = 0.0f;
        }
        else if (p.vx < 0.0f)
        {
            p.vx += MOVE_FRICTION * dt;
            if (p.vx > 0.0f) p.vx = 0.0f;
        }
    }

    // Limit horizontal speed
    if (p.vx >  MAX_SPEED) p.vx =  MAX_SPEED;
    if (p.vx < -MAX_SPEED) p.vx = -MAX_SPEED;

    // -------- Jump --------
    bool jumpPressed = IsKeyPressed(KEY_SPACE) ||
                       IsKeyPressed(KEY_W)     ||
                       IsKeyPressed(KEY_UP);

    if (jumpPressed && p.onGround)
    {
        p.vy = JUMP_SPEED;
        p.onGround = false;
    }

    // -------- Gravity --------
    p.vy += GRAVITY * dt;

    // -------- Position update --------
    p.x += p.vx * dt;
    p.y += p.vy * dt;

    // Ground collision
    if (p.y + p.height >= groundY)
    {
        p.y = groundY - p.height;
        p.vy = 0.0f;
        p.onGround = true;
    }
    else
    {
        p.onGround = false;
    }

    // Keep inside screen horizontally
    float screenW = (float)GetScreenWidth();
    if (p.x < 0.0f) p.x = 0.0f;
    if (p.x + p.width > screenW) p.x = screenW - p.width;

    // -------- “Bring character to life” (bobbing effect) --------
    // animTime always increases – used for small sine movement
    bool movingOrAir = (std::fabs(p.vx) > 5.0f) || !p.onGround;

    if (movingOrAir)
        p.animTime += dt * 5.0f;   // faster animation when running/jumping
    else
        p.animTime += dt * 2.0f;   // slower animation when idle

    if (p.animTime > 1000.0f) p.animTime -= 1000.0f;
}

// ---------------------------------------------------------------
//  DrawPlayer : draws the character with foot offset + bobbing
// ---------------------------------------------------------------
void DrawPlayer(const Player &p)
{
    // Whole image as source (only one frame)
    Rectangle src;
    src.x = 0.0f;
    src.y = 0.0f;
    src.width  = (float)p.texture.width;
    src.height = (float)p.texture.height;

    // Size on screen
    Rectangle dest;
    dest.width  = p.width;
    dest.height = p.height;

    // Position: center the sprite + apply FEET_OFFSET so the feet visually touch ground
    dest.x = p.x + dest.width  / 2.0f;
    dest.y = p.y + dest.height / 2.0f + FEET_OFFSET;

    // --- Bobbing effect ---
    float bob = 0.0f;
    bool running = (std::fabs(p.vx) > 5.0f) && p.onGround;

    if (running)
    {
        // When running – bigger up/down movement, feels like steps
        bob = std::sinf(p.animTime * 15.0f) * 4.0f; // increase to 6 for stronger effect
    }
    else if (p.onGround)
    {
        // Idle – small “breathing” movement
        bob = std::sinf(p.animTime * 4.0f) * 1.5f;
    }
    else
    {
        // In air – softer bobbing
        bob = std::sinf(p.animTime * 8.0f) * 2.0f;
    }

    dest.y += bob;

    // --- Rotation response to movement ---
    float angle = 0.0f;

    if (!p.onGround)
    {
        // In air – slight tilt depending on vertical speed
        angle = -p.vy * 0.03f;
        if (angle >  15.0f) angle =  15.0f;
        if (angle < -15.0f) angle = -15.0f;
    }
    else if (running)
    {
        // Running on ground – gentle left/right swinging
        angle = std::sinf(p.animTime * 10.0f) * 6.0f;
    }

    Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

    DrawTexturePro(p.texture, src, dest, origin, angle, WHITE);
}

// ---------------------------------------------------------------
//  UnloadPlayer : release the texture
// ---------------------------------------------------------------
void UnloadPlayer(Player &p)
{
    if (p.texture.id != 0)
    {
        UnloadTexture(p.texture);
        p.texture.id = 0;
    }
}