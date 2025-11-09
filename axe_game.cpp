#include "raylib.h"
#include <cmath>

int main()
{
    // --- Window ---
    const int WIDTH  = 800;
    const int HEIGHT = 450;
    InitWindow(WIDTH, HEIGHT, "Ariel's crazy dodge");

    SetTargetFPS(60);

    // --- Audio ---
    InitAudioDevice();
    Sound sPoint = LoadSound("point.wav");      // score sound
    Sound sHit   = LoadSound("GameOver.wav");  // hit sound

    // --- Player (circle) ---
    float circleX = 150.0f;
    float circleY = HEIGHT / 2.0f;
    const float R = 25.0f;
    const float PLAYER_SPEED = 300.0f; // pixels / second

    // --- Axe (now moves in ALL directions) ---
    const float AXE_SIZE = 50.0f;
    float axeX = WIDTH / 2.0f;
    float axeY = 100.0f;
    float axeVX = 250.0f;  // x velocity (can be + or -)
    float axeVY = 180.0f;  // y velocity

    // --- Laser obstacle ---
    bool  laserActive   = false;
    float laserY        = 0.0f;   // vertical position of the laser line
    float laserTimer    = 0.0f;   // how long the laser is visible
    float laserCooldown = 2.0f;   // time until next laser appears
    const float LASER_THICKNESS = 12.0f;
    const float LASER_DURATION  = 0.6f;  // seconds
    const float LASER_COOLDOWN_MIN = 1.5f;
    const float LASER_COOLDOWN_MAX = 3.0f;

    // --- Game state ---
    bool  gameOver   = false;
    int   score      = 0;
    float secCounter = 0.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // RESTART
        if (gameOver && IsKeyPressed(KEY_R))
        {
            circleX = 150.0f;
            circleY = HEIGHT / 2.0f;
            axeX    = WIDTH / 2.0f;
            axeY    = 100.0f;
            axeVX   = 250.0f;
            axeVY   = 180.0f;

            laserActive   = false;
            laserCooldown = 2.0f;
            laserTimer    = 0.0f;

            score      = 0;
            secCounter = 0.0f;
            gameOver   = false;
        }

        if (!gameOver)
        {
            // -------- PLAYER MOVEMENT (WASD) --------
            if (IsKeyDown(KEY_D)) circleX += PLAYER_SPEED * dt;
            if (IsKeyDown(KEY_A)) circleX -= PLAYER_SPEED * dt;
            if (IsKeyDown(KEY_S)) circleY += PLAYER_SPEED * dt;
            if (IsKeyDown(KEY_W)) circleY -= PLAYER_SPEED * dt;

            // keep inside screen
            if (circleX < R) circleX = R;
            if (circleX > WIDTH - R) circleX = WIDTH - R;
            if (circleY < R) circleY = R;
            if (circleY > HEIGHT - R) circleY = HEIGHT - R;

            // -------- AXE MOVEMENT (2D bounce) --------
            axeX += axeVX * dt;
            axeY += axeVY * dt;

            // bounce on walls
            if (axeX < 0)
            {
                axeX = 0;
                axeVX *= -1;
            }
            if (axeX > WIDTH - AXE_SIZE)
            {
                axeX = WIDTH - AXE_SIZE;
                axeVX *= -1;
            }
            if (axeY < 0)
            {
                axeY = 0;
                axeVY *= -1;
            }
            if (axeY > HEIGHT - AXE_SIZE)
            {
                axeY = HEIGHT - AXE_SIZE;
                axeVY *= -1;
            }

            // -------- LASER LOGIC --------
            if (!laserActive)
            {
                laserCooldown -= dt;
                if (laserCooldown <= 0.0f)
                {
                    laserActive = true;
                    laserTimer  = LASER_DURATION;
                    // pick a random Y for the laser line
                    laserY = (float)GetRandomValue(50, HEIGHT - 50);
                }
            }
            else
            {
                laserTimer -= dt;
                if (laserTimer <= 0.0f)
                {
                    laserActive = false;
                    // next laser in a random time
                    laserCooldown = LASER_COOLDOWN_MIN +
                        ((float)GetRandomValue(0, 100) / 100.0f) * (LASER_COOLDOWN_MAX - LASER_COOLDOWN_MIN);
                }
            }

            // -------- SCORING & DIFFICULTY --------
            secCounter += dt;
            if (secCounter >= 1.0f)
            {
                secCounter = 0.0f;
                score += 1;
                PlaySound(sPoint);

                // every 5 points, speed up axe a bit
                if (score % 5 == 0)
                {
                    float factor = 1.15f;
                    axeVX *= factor;
                    axeVY *= factor;
                }
            }

            // -------- COLLISION WITH AXE --------
            float lCircleX = circleX - R;
            float rCircleX = circleX + R;
            float uCircleY = circleY - R;
            float bCircleY = circleY + R;

            float lAxeX = axeX;
            float rAxeX = axeX + AXE_SIZE;
            float uAxeY = axeY;
            float bAxeY = axeY + AXE_SIZE;

            bool collideAxe = (bAxeY >= uCircleY) &&
                              (bCircleY >= uAxeY) &&
                              (rAxeX >= lCircleX) &&
                              (rCircleX >= lAxeX);

            // -------- COLLISION WITH LASER --------
            bool collideLaser = false;
            if (laserActive)
            {
                // laser is a horizontal rectangle across the whole width
                float laserTop    = laserY - LASER_THICKNESS * 0.5f;
                float laserBottom = laserY + LASER_THICKNESS * 0.5f;

                // if any part of the circle vertical span overlaps the laser span
                if (bCircleY >= laserTop && uCircleY <= laserBottom)
                    collideLaser = true;
            }

            if (collideAxe || collideLaser)
            {
                gameOver = true;
                PlaySound(sHit);
            }
        }

        // ------------- DRAW -------------
        BeginDrawing();
        ClearBackground(BLACK);

        if (gameOver)
        {
            DrawText("GAME OVER!", 260, 160, 50, RED);
            DrawText(TextFormat("Score: %d", score), 330, 220, 30, RAYWHITE);
            DrawText("Press R to restart", 290, 270, 20, GRAY);
        }
        else
        {
            // draw background "running" stripes to make it feel dynamic
            for (int x = 0; x < WIDTH; x += 40)
                DrawRectangleLines(x, 0, 40, HEIGHT, Fade(DARKGRAY, 0.2f));

            // player
            DrawCircle((int)circleX, (int)circleY, R, SKYBLUE);

            // axe
            DrawRectangle((int)axeX, (int)axeY, (int)AXE_SIZE, (int)AXE_SIZE, RED);

            // laser
            if (laserActive)
            {
                DrawRectangle(0, (int)(laserY - LASER_THICKNESS / 2),
                              WIDTH, (int)LASER_THICKNESS,
                              Color{ 255, 0, 255, 160 }); // purple, semi-transparent
            }

            // HUD
            DrawText(TextFormat("Score: %d", score), 20, 20, 25, RAYWHITE);
            DrawText("Move: WASD | Avoid axe + laser | R = restart", 20, HEIGHT - 30, 18, GRAY);
        }

        EndDrawing();
    }

    UnloadSound(sPoint);
    UnloadSound(sHit);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
