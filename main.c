#include "raylib.h"
#include "raymath.h"
#include "stddef.h"

/* TODO
 * STRETCH
 * - FEATURE: At the eye's zenith, background turns white and eye opens.
 * - FEATURE: While the eye is open, hambert stops.
 * - FEATURE: If Hambert isn't in behind a panel when the eye is open, game over.
 *
 * STRETCH
 * - FEATURE: Put animated texture on hambert.
 * - FEATURE: Put static texture on pecan.
 * - FEATURE: Hambert gets wider and faster as he east.
 * - FEATURE: Animation for losing and winning.
 */

// Util
//--------------------------------------------------------------------------------------
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
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
const int MAX_PANELS = 4;
const float PANEL_WIDTH = 2;
const float PANEL_HEIGHT = 2;
const float PANEL_DEPTH = 0.1;
const float PANEL_Y = 1; // PANEL_HEIGHT / 2;
const float PANEL_STARTING_Z = -20;
const float PANEL_MAX_Z = 1;
const float PANEL_SPEED = 10;
const Color PANEL_COLOR = DARKGREEN;
const Color PANEL_WIRE_COLOR = GREEN;

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
    while (filled < MIN_PANELS || filled > MAX_PANELS)
        filled = _PanelsFill(panels);

    panels->z = PANEL_STARTING_Z;
}

void PanelsMove(Panels *panels, float delta)
{
    panels->z += PANEL_SPEED * delta;
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

size_t PanelsEmptySlot(Panels *panels)
{
    size_t len = 0;
    size_t buffer[PANEL_SLOTS];

    for (size_t i = 0; i < PANEL_SLOTS; i++)
    {
        if (!panels->alive[i])
        {
            buffer[len] = i;
            len++;
        }
    };

    size_t idx = GetRandomValue(0, len - 1);
    return buffer[idx];
}

void PanelsDraw(Panels *panels)
{
    for (size_t i = 0; i < PANEL_SLOTS; i++)
    {
        if (panels->alive[i])
        {
            DrawCube(_PanelsPos(panels, i), PANEL_WIDTH, PANEL_HEIGHT, PANEL_DEPTH, PANEL_COLOR);
            DrawCubeWires(_PanelsPos(panels, i), PANEL_WIDTH, PANEL_HEIGHT, PANEL_DEPTH, PANEL_WIRE_COLOR);
        }
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

// Eye
//--------------------------------------------------------------------------------------
const float EYE_RADIUS = 32;
const float EYE_ORBIT_RADIUS = 100;
const float EYE_SPEED = 1;

void EyeMove(float *angle, float delta)
{
    *angle += EYE_SPEED * delta;
    if (*angle > 2 * PI)
        *angle -= 2 * PI;
    TraceLog(LOG_INFO, TextFormat("angle = %f", *angle));
}

void EyeDraw(float angle)
{
    if (angle > 1.25 * PI && angle < 2 * PI - 0.15 * PI)
    {
        float x = cosf(angle) * EYE_ORBIT_RADIUS + SCREEN_WIDTH / 2;
        float y = sinf(angle) * EYE_ORBIT_RADIUS + SCREEN_HEIGHT / 2;
        DrawCircle(x, y, 32, RAYWHITE);
    }
}
//--------------------------------------------------------------------------------------

// Game
//--------------------------------------------------------------------------------------
const float GRID_SPACING = 1;
const Color GRID_COLOR = DARKGREEN;

typedef enum State
{
    START,
    PLAYING,
    GAME_OVER,
} State;

typedef struct Game
{
    Panels panels;
    float hambert;
    size_t pecan;
    float eyeAngle;
    int score;
    State state;
} Game;

void GameInit(Game *game)
{
    PanelsInit(&game->panels);
    HambertInit(&game->hambert);
    game->pecan = PanelsEmptySlot(&game->panels);
    game->score = 0;
    game->eyeAngle = 0;
    game->state = START;
}

void GameReset(Game *game)
{
    GameInit(game);
    game->state = PLAYING;
    game->hambert = 0;
}

void GameUpdate(Game *game, float delta)
{
    switch (game->state)
    {
    case START:
    {
        if (IsKeyPressed(KEY_SPACE))
            game->state = PLAYING;
    }
    break;
    case PLAYING:
    {
        PanelsMove(&game->panels, delta);
        HambertMove(&game->hambert, delta);
        EyeMove(&game->eyeAngle, delta);
        BoundingBox hambertBox = HambertBox(game->hambert);

        if (CheckCollisionBoxes(hambertBox, PecanBox(game->panels.z, game->pecan)))
        {
            game->score++;
            game->pecan = 10000;
        }

        if (CheckPanelCollisions(&game->panels, hambertBox))
            game->state = GAME_OVER;

        if (game->panels.z > PANEL_MAX_Z)
        {
            PanelsInit(&game->panels);
            game->pecan = PanelsEmptySlot(&game->panels);
        }
    }
    break;

    case GAME_OVER:
        if (IsKeyPressed(KEY_SPACE))
            GameReset(game);
        break;
    }
}

void _GameDrawXLine(float z)
{
    Vector3 start = {-WIDTH / 2, 0, z};
    Vector3 end = {WIDTH / 2, 0, z};
    DrawLine3D(start, end, GRID_COLOR);
}

void _GameDrawZLine(float x)
{
    Vector3 start = {x, 0, -100.0};
    Vector3 end = {x, 0, 200.0};
    DrawLine3D(start, end, GRID_COLOR);
}

void _GameDrawGrid(float z)
{
    for (int i = -20; i <= 20; i++)
    {
        float lineZ = z + i * GRID_SPACING;
        _GameDrawXLine(lineZ);
    }

    for (int i = -WIDTH; i < WIDTH; i++)
        _GameDrawZLine(i * GRID_SPACING);
}

void _GameDrawWall(float x)
{
    Vector3 pos = {x, PANEL_HEIGHT / 2, 0};
    DrawCube(pos, PANEL_DEPTH, PANEL_HEIGHT, 100.0, PANEL_COLOR);
    DrawCubeWires(pos, PANEL_DEPTH, PANEL_HEIGHT, 100.0, PANEL_WIRE_COLOR);
}

void GameDrawUnderlay(Game *game)
{
    switch (game->state)
    {
    case PLAYING:
        EyeDraw(game->eyeAngle);
        break;

    default:
        break;
    }
}

void GameDraw(Game *game)
{
    _GameDrawGrid(game->panels.z);
    _GameDrawWall(-WIDTH / 2);
    _GameDrawWall(WIDTH / 2);
    PanelsDraw(&game->panels);
    PecanDraw(game->panels.z, game->pecan);
    HambertDraw(game->hambert);
}

void GameDrawOverlay(Game *game)
{
    switch (game->state)
    {
    case START:
    {
        const char *message = "HAMBERT'S QUEST";
        int messageWidth = MeasureText(message, 60);
        float messageX = SCREEN_WIDTH / 2 - messageWidth / 2;
        float messageY = SCREEN_HEIGHT / 4 - 60;
        DrawText(message, messageX, messageY, 60, RAYWHITE);

        const char *score = "Press 'Space' to Start";
        int scoreWidth = MeasureText(score, 40);
        float scoreX = SCREEN_WIDTH / 2 - scoreWidth / 2;
        float scoreY = SCREEN_HEIGHT * 3 / 4;
        DrawText(score, scoreX, scoreY, 40, RAYWHITE);
    }
    break;
    case PLAYING:
    {
        DrawText(TextFormat("Pecans: %d", game->score), 10, 10, 20, RAYWHITE);
    }
    break;

    case GAME_OVER:
    {
        DrawText("Pres 'Space' to restart.", 10, 10, 20, RAYWHITE);

        const char *message = "G A M E  O V E R";
        int messageWidth = MeasureText(message, 60);
        float messageX = SCREEN_WIDTH / 2 - messageWidth / 2;
        float messageY = SCREEN_HEIGHT / 2 - 60;
        DrawText(message, messageX, messageY, 60, RAYWHITE);

        const char *score = TextFormat("Pecans: %d", game->score);
        int scoreWidth = MeasureText(score, 40);
        float scoreX = SCREEN_WIDTH / 2 - scoreWidth / 2;
        float scoreY = SCREEN_HEIGHT / 2;
        DrawText(score, scoreX, scoreY, 40, RAYWHITE);
    }
    break;
    }
}
//--------------------------------------------------------------------------------------

int main(void)
{
    // SETUP
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hambert's Quest");

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

        GameDrawUnderlay(&game);

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