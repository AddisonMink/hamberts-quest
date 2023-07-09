#include "raylib.h"
#include "stddef.h"

/* TODO
 * - BUG: Max panel limit not working.
 * - FEATURE: Randomly spawn pecan in open slot.
 * - FEATURE: Collision with panels causes game over.
 * - FEATURE: An eye circles over the horizon.
 * - FEATURE: At the eye's zenith, background turns white and eye opens.
 * - FEATURE: While the eye is open, hambert stops.
 * - FEATURE: If Hambert isn't in behind a panel when the eye is open, game over.
 * - FEATURE: Draw sides of room.
 * - FEATURE: Game over screen with option to restart.
 * - FEATURE: Start screen.
 */

// Util
//--------------------------------------------------------------------------------------
const Vector3 CAMERA_POSITION = {0.0f, 1.0f, 10.0f};
const float WIDTH = 10;

BoundingBox MakeBox(Vector3 pos, float width, float height, float depth)
{
    BoundingBox box;
    box.min.x = pos.x - (width / 2.0f);
    box.min.y = pos.y - (height / 2.0f);
    box.min.z = pos.z - (depth / 2.0f);
    box.max.x = pos.x + (width / 2.0f);
    box.max.y = pos.y + (height / 2.0f);
    box.max.z = pos.z + (depth / 2.0f);
    return box;
}
//--------------------------------------------------------------------------------------

// Panels
//--------------------------------------------------------------------------------------
const size_t PANEL_SLOTS = 5;
const int PANEL_SPAWN_CHANCE = 50;
const int MIN_PANELS = 2;
const int MAX_PANELS = PANEL_SLOTS - 1;
const float PANEL_WIDTH = 2;
const float PANEL_HEIGHT = 2;
const float PANEL_DEPTH = 0.1;
const float PANEL_Y = 1; // PANEL_HEIGHT / 2;
const float PANEL_STARTING_Z = -20;
const float PANEL_MAX_Z = 1;
const float PANEL_SPEED = 10;
const Color PANEL_COLOR = RAYWHITE;

typedef struct Panels
{
    bool alive[PANEL_SLOTS];
    float z;
} Panels;

int _PanelsFill(Panels *panels)
{
    int filled = 0;
    for (size_t i = 0; i < PANEL_SLOTS; i++)
    {
        bool alive = GetRandomValue(0, 100) < PANEL_SPAWN_CHANCE;
        panels->alive[i] = alive;
        if (alive)
            filled++;
    }
    return filled;
}

void PanelsInit(Panels *panels)
{
    int filled = 0;
    while (filled < MIN_PANELS || filled >= MAX_PANELS)
        filled = _PanelsFill(panels);

    panels->alive[0] = true;
    panels->alive[4] = true;
    panels->z = PANEL_STARTING_Z;
}

void PanelsMove(Panels *panels, float delta)
{
    panels->z += PANEL_SPEED * delta;
    if (panels->z > PANEL_MAX_Z)
        PanelsInit(panels);
}

float PanelsX(size_t slot)
{
    float offset = -PANEL_WIDTH * (PANEL_SLOTS / 2);
    return slot * PANEL_WIDTH + offset;
}

Vector3 _PanelsPos(Panels *panels, size_t slot)
{
    float offset = -PANEL_WIDTH * (PANEL_SLOTS / 2);
    float x = PanelsX(slot);
    return (Vector3){x, PANEL_Y, panels->z};
}

BoundingBox _PanelsBox(Panels *panels, size_t slot)
{
    Vector3 pos = _PanelsPos(panels, slot);
    return MakeBox(pos, PANEL_WIDTH, PANEL_HEIGHT, PANEL_DEPTH);
}

bool CheckPanelCollisions(Panels *panels, BoundingBox box)
{
    for (size_t i = 0; i < PANEL_SLOTS; i++)
        if (panels->alive[i] && CheckCollisionBoxes(box, _PanelsBox(panels, i)))
            return true;
    return false;
}

Vector3 PanelsDraw(Panels *panels)
{
    for (size_t i = 0; i < PANEL_SLOTS; i++)
    {
        if (!panels->alive[i])
            continue;
        DrawCube(_PanelsPos(panels, i), PANEL_WIDTH, PANEL_HEIGHT, PANEL_DEPTH, PANEL_COLOR);
    }
}
//--------------------------------------------------------------------------------------

// Hambert
//--------------------------------------------------------------------------------------
const float HAMBERT_WIDTH = 1.5;
const float HAMBERT_HEIGHT = 0.75;
const float HAMBERT_DEPTH = 0.1;
const float HAMBERT_MIN_X = -4.25; // -WIDTH / 2 + HAMBERT_WIDTH / 2;
const float HAMBERT_MAX_X = 4.25;  // WIDTH / 2 - HAMBERT_WIDTH / 2;
const float HAMBERT_Y = 0.375;     // HAMBERT_HEIGHT / 2;
const float HAMBERT_Z = 0;
const float HAMBERT_SPEED = 5;
const Color HAMBERT_COLOR = BEIGE;

void HambertInit(float *x)
{
    x = 0;
}

void HambertMove(float *x, float delta)
{
    float dx = 0;
    if (IsKeyDown(KEY_A))
        dx -= HAMBERT_SPEED;
    if (IsKeyDown(KEY_D))
        dx += HAMBERT_SPEED;
    dx = dx * delta;

    *x += dx;
    if (*x < HAMBERT_MIN_X)
        *x = HAMBERT_MIN_X;
    if (*x > HAMBERT_MAX_X)
        *x = HAMBERT_MAX_X;
}

BoundingBox HambertBox(float x)
{
    Vector3 pos = {x, HAMBERT_Y, HAMBERT_Z};
    return MakeBox(pos, HAMBERT_WIDTH, HAMBERT_HEIGHT, HAMBERT_DEPTH);
}

void HambertDraw(float x)
{
    Vector3 pos = {x, HAMBERT_Y, HAMBERT_Z};
    DrawCube(pos, HAMBERT_WIDTH, HAMBERT_HEIGHT, HAMBERT_DEPTH, HAMBERT_COLOR);
}
//--------------------------------------------------------------------------------------

// Pecan
//--------------------------------------------------------------------------------------
const float PECAN_WIDTH = 0.5;
const float PECAN_HEIGHT = 0.5;
const float PECAN_DEPTH = 0.1;
const float PECAN_Y = 0.25; // PECAN_HEIGHT / 2;
const Color PECAN_COLOR = BROWN;

Vector3 _PecanPos(float z, size_t slot)
{
    return (Vector3){PanelsX(slot), PECAN_Y, z};
}

BoundingBox PecanBox(float z, size_t slot)
{
    return MakeBox(_PecanPos(z, slot), PECAN_WIDTH, PECAN_HEIGHT, PECAN_DEPTH);
}

void PecanDraw(float z, size_t slot)
{
    Vector3 pos = _PecanPos(z, slot);
    DrawCube(pos, PECAN_WIDTH, PECAN_HEIGHT, PECAN_DEPTH, PECAN_COLOR);
}

//--------------------------------------------------------------------------------------

// Game
//--------------------------------------------------------------------------------------
const Color GRID_COLOR = DARKGREEN;

typedef struct Game
{
    Panels panels;
    float hambert;
    size_t pecan;
    int score;
} Game;

void GameInit(Game *game)
{
    PanelsInit(&game->panels);
    HambertInit(&game->hambert);
    game->pecan = 2;
    game->score = 0;
}

void GameUpdate(Game *game, float delta)
{
    PanelsMove(&game->panels, delta);
    HambertMove(&game->hambert, delta);
    BoundingBox hambertBox = HambertBox(game->hambert);

    if (CheckCollisionBoxes(hambertBox, PecanBox(game->panels.z, game->pecan)))
    {
        game->score++;
    }
}

void GameDraw(Game *game)
{
    PanelsDraw(&game->panels);
    PecanDraw(game->panels.z, game->pecan);
    HambertDraw(game->hambert);
    DrawGrid(10, 1.0f);
}

void GameDrawOverlay(Game *game)
{
    DrawText(TextFormat("SCORE: %d", game->score), 10, 10, 40, RAYWHITE);
}
//--------------------------------------------------------------------------------------

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    // SETUP
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    Camera3D camera = {0};
    camera.position = CAMERA_POSITION;
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Game game;
    GameInit(&game);

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        GameUpdate(&game, GetFrameTime());
        camera.position.x = game.hambert;
        camera.target.x = game.hambert;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);
        GameDraw(&game);
        EndMode3D();

        GameDrawOverlay(&game);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}