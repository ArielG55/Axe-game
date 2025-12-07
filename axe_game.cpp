#include "raylib.h"
#include "Player.h"

// ---------------- Game types ----------------

enum GameState {
    PLAYING,
    GAME_OVER,
    WIN
};

struct Obstacle {
    float x, y, w, h;
    bool  active;
};

// ---------------- main ----------------

int main()
{
    // --- Window ---
    const int WIDTH  = 800;
    const int HEIGHT = 450;
    InitWindow(WIDTH, HEIGHT, "Ariel's Runner");
    SetTargetFPS(60);

    // --- Audio ---
    InitAudioDevice();

    // scary background theme – loops while PLAYING
    Sound sTheme = LoadSound("Theme.wav");
    // game-over hit sound
    Sound sHit   = LoadSound("GameOver.wav");

    // start theme immediately
    PlaySound(sTheme);

    // --- Ground ---
    const float GROUND_Y = HEIGHT - 80.0f;

    // --- Player ---
    Player player;
    InitPlayer(player, GROUND_Y);

    // --- Obstacles ---
    const int MAX_OBSTACLES = 3;
    Obstacle obstacles[MAX_OBSTACLES];

    float baseSpeed    = 300.0f;
    float currentSpeed = baseSpeed;

    // initial obstacle positions
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i].w = 40.0f;
        obstacles[i].h = 60.0f;
        obstacles[i].x = (float)WIDTH + i * 300.0f;
        obstacles[i].y = GROUND_Y - obstacles[i].h;
        obstacles[i].active = true;
    }

    // --- Game state ---
    GameState state      = PLAYING;
    int       score      = 0;
    float     scoreTimer = 0.0f;
    const int TARGET_SCORE = 1000;

    // --- Background scroll ---
    float bgScroll = 0.0f;

    // ---------------- Game loop ----------------
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // keep theme looping while PLAYING (if it ended)
        if (state == PLAYING && !IsSoundPlaying(sTheme))
        {
            PlaySound(sTheme);
        }

        // -------- restart on R when not playing --------
        if (state != PLAYING && IsKeyPressed(KEY_R))
        {
            // reset player
            InitPlayer(player, GROUND_Y);

            // reset speed & score
            currentSpeed = baseSpeed;
            score        = 0;
            scoreTimer   = 0.0f;

            // reset obstacles
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                obstacles[i].w = 40.0f;
                obstacles[i].h = 60.0f;
                obstacles[i].x = (float)WIDTH + i * 300.0f;
                obstacles[i].y = GROUND_Y - obstacles[i].h;
                obstacles[i].active = true;
            }

            // reset background scroll
            bgScroll = 0.0f;

            // stop game-over sound if still playing
            StopSound(sHit);

            // restart theme from the beginning
            StopSound(sTheme);
            PlaySound(sTheme);

            state = PLAYING;
        }

        // ---------------- UPDATE (PLAYING) ----------------
        if (state == PLAYING)
        {
            UpdatePlayer(player, dt, GROUND_Y);

            // scroll background
            bgScroll -= currentSpeed * 0.4f * dt;
            if (bgScroll <= -80.0f) bgScroll += 80.0f;

            // move obstacles
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (!obstacles[i].active) continue;

                obstacles[i].x -= currentSpeed * dt;

                // if off-screen → respawn to the right
                if (obstacles[i].x + obstacles[i].w < 0.0f)
                {
                    obstacles[i].x = (float)WIDTH + (float)GetRandomValue(200, 500);
                    obstacles[i].y = GROUND_Y - obstacles[i].h;
                }
            }

            // scoring (no sound here)
            scoreTimer += dt;
            if (scoreTimer >= 0.1f && score < TARGET_SCORE)
            {
                scoreTimer -= 0.1f;
                score += 1;

                // make game a bit faster every 100 points
                if (score % 100 == 0)
                    currentSpeed *= 1.10f;
            }

            if (score >= TARGET_SCORE)
            {
                score = TARGET_SCORE;
                state = WIN;

                // you can stop the theme on win if you want:
                StopSound(sTheme);
            }

            // -------- collision check --------
            Rectangle playerRect = { player.x, player.y, player.width, player.height };

            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (!obstacles[i].active) continue;

                Rectangle obsRect = {
                    obstacles[i].x,
                    obstacles[i].y,
                    obstacles[i].w,
                    obstacles[i].h
                };

                if (CheckCollisionRecs(playerRect, obsRect))
                {
                    state = GAME_OVER;

                    // stop theme and play hit once
                    StopSound(sTheme);
                    PlaySound(sHit);
                    break;
                }
            }
        }

        // ---------------- DRAW ----------------
        BeginDrawing();
        ClearBackground(DARKBLUE);

        // ---- SCARY BACKGROUND ----

        // blood-red moon
        DrawCircle(WIDTH - 90, 80, 45, (Color){ 200, 20, 20, 255 });
        DrawCircle(WIDTH - 80, 75, 30, DARKBLUE); // small bite

        // far mountains (dark silhouettes)
        DrawTriangle(
            (Vector2){  50 + bgScroll * 0.2f, GROUND_Y },
            (Vector2){ 200 + bgScroll * 0.2f, 220 },
            (Vector2){ 350 + bgScroll * 0.2f, GROUND_Y },
            (Color){ 10, 10, 40, 255 }
        );
        DrawTriangle(
            (Vector2){ 300 + bgScroll * 0.2f, GROUND_Y },
            (Vector2){ 480 + bgScroll * 0.2f, 200 },
            (Vector2){ 660 + bgScroll * 0.2f, GROUND_Y },
            (Color){ 5, 5, 25, 255 }
        );

        // little dark rocks instead of crosses
        for (int i = 0; i < 6; i++)
        {
            float rx = (float)(i * 140) + bgScroll * 0.5f;
            float ry = GROUND_Y - 8.0f;
            DrawEllipse(rx, ry, 10, 5, (Color){ 20, 20, 20, 255 });
        }

        // creepy dead trees
        for (int i = 0; i < 3; i++)
        {
            float tx = (float)(120 + i * 240) + bgScroll * 0.3f;
            float ty = GROUND_Y - 60.0f;
            DrawRectangle(tx, ty, 8, 60, BLACK);
            DrawLine(tx,     ty,     tx - 20, ty - 25, BLACK);
            DrawLine(tx + 8, ty - 5, tx + 28, ty - 30, BLACK);
        }

        // ground
        DrawRectangle(
            0, (int)GROUND_Y,
            WIDTH, HEIGHT - (int)GROUND_Y,
            (Color){ 10, 60, 10, 255 }
        );

        // vertical “blood mist” stripes
        for (int x = -80; x < WIDTH + 80; x += 80)
        {
            DrawLine(
                x + (int)bgScroll, 0,
                x + (int)bgScroll, (int)GROUND_Y,
                Fade((Color){ 120, 0, 0, 255 }, 0.15f)
            );
        }

        // Obstacles
        for (int i = 0; i < MAX_OBSTACLES; i++)
        {
            if (!obstacles[i].active) continue;
            DrawRectangle(
                (int)obstacles[i].x,
                (int)obstacles[i].y,
                (int)obstacles[i].w,
                (int)obstacles[i].h,
                RED
            );
        }

        // Player
        DrawPlayer(player);

        // HUD
        DrawText(
            TextFormat("Score: %d / %d", score, TARGET_SCORE),
            20, 20, 24, RAYWHITE
        );
        DrawText(
            "Move: A/D or Arrows | Jump: SPACE/W/UP | R = restart",
            20, 50, 18, RAYWHITE
        );

        // overlays
        if (state == GAME_OVER)
        {
            DrawText("GAME OVER!", 260, 150, 50, RED);
            DrawText(
                TextFormat("Final Score: %d", score),
                320, 210, 30, RAYWHITE
            );
            DrawText("Press R to try again", 290, 260, 22, RAYWHITE);
        }
        else if (state == WIN)
        {
            DrawText("YOU WIN!", 280, 150, 56, GOLD);
            DrawText("You reached 1000 points!", 250, 210, 28, RAYWHITE);
            DrawText("Press R to play again", 290, 260, 22, RAYWHITE);
        }

        EndDrawing();
    }

    // ---------------- Cleanup ----------------
    StopSound(sTheme);
    StopSound(sHit);
    UnloadSound(sTheme);
    UnloadSound(sHit);

    UnloadPlayer(player);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}