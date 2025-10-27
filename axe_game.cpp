#include "raylib.h"

int main()
{
    // --- Window ---
    const int WIDTH  = 800;
    const int HEIGHT = 450;
    InitWindow(WIDTH, HEIGHT, "Ariel's window");
    SetTargetFPS(60);

    // --- Audio ---
    InitAudioDevice();                           // must come before LoadSound
    Sound sPoint = LoadSound("point.wav");       // plays when score increases
    Sound sHit   = LoadSound("GameOver.wav");         // plays once on collision

    // --- Player (circle) ---
    int  circleX = 200;
    int  circleY = 200;
    const int R  = 25;

    // --- Enemy (axe) ---
    const int AXE_W = 50;
    const int AXE_H = 50;
    int   axeX = 400;
    int   axeY = 0;
    float axeVel = 10.0f;                        // pixels per frame, sign = direction

    // --- Game state ---
    bool  gameOver    = false;
    int   score       = 0;
    float secCounter  = 0.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();               // seconds since last frame

        // ------- Restart -------
        if (gameOver && IsKeyPressed(KEY_R))
        {
            circleX   = 200;
            circleY   = 200;
            axeX      = 400;
            axeY      = 0;
            axeVel    = 10.0f;
            score     = 0;
            secCounter= 0.0f;
            gameOver  = false;
        }

        // ------- Update (only when playing) -------
        if (!gameOver)
        {
            // Move axe (bounce at top/bottom)
            axeY += (int)axeVel;
            if (axeY < 0 || axeY > HEIGHT - AXE_H)
                axeVel *= -1.0f;

            // Move player (A/D), keep within bounds
            if (IsKeyDown(KEY_D) && circleX < WIDTH - R)  circleX += 10;
            if (IsKeyDown(KEY_A) && circleX > R)          circleX -= 10;

            // Score: +1 per real-time second
            secCounter += dt;
            if (secCounter >= 1.0f)
            {
                score += 1;
                secCounter = 0.0f;
                PlaySound(sPoint);                         // ding!
                // difficulty: every 5 points, speed up a bit (preserve sign)
                if (score % 5 == 0)
                    axeVel += (axeVel > 0 ? 1.5f : -1.5f);
            }

            // Collision (AABB vs circle bounds as AABB)
            int lCircleX = circleX - R;
            int rCircleX = circleX + R;
            int uCircleY = circleY - R;
            int bCircleY = circleY + R;

            int lAxeX = axeX;
            int rAxeX = axeX + AXE_W;
            int uAxeY = axeY;
            int bAxeY = axeY + AXE_H;

            bool collide = (bAxeY >= uCircleY) &&
                           (bCircleY >= uAxeY) &&
                           (rAxeX  >= lCircleX) &&
                           (rCircleX>= lAxeX);

            if (collide)
            {
                gameOver = true;
                PlaySound(sHit);                           // boom!
            }
        }

        // ------- Draw -------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (gameOver)
        {
            DrawText("GAME OVER!", 320, 170, 40, RED);
            DrawText(TextFormat("Final Score: %i", score), 340, 220, 22, DARKGRAY);
            DrawText("Press R to restart", 340, 255, 20, GRAY);
        }
        else
        {
            DrawCircle(circleX, circleY, (float)R, BLUE);
            DrawRectangle(axeX, axeY, AXE_W, AXE_H, RED);
            DrawText(TextFormat("Score: %i", score), 20, 20, 24, DARKGRAY);
            DrawText(TextFormat("Speed: %.1f", axeVel > 0 ? axeVel : -axeVel), 20, 50, 20, GRAY);
        }

        EndDrawing();
    }

    // --- Cleanup ---
    UnloadSound(sPoint);
    UnloadSound(sHit);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}