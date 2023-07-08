#include "raylib.h"
#include "stddef.h"

const Vector3 CAMERA_POSITION = {0.0f, 2.5f, -10.0f};

const int NUM_PANELS = 2;
const float PANEL_WIDTH = 2.0f;
const float PANEL_HEIGHT = 2.0f;
const float PANEL_DEPTH = 0.1f;
const float PANEL_SPEED = 20.0f;
const int PANEL_START_Z = 20;
const int PANEL_MIN_X = 4;
const int PANEL_MAX_X = -6;
const Color PANEL_COLOR = RED;

const float HAMBERT_WIDTH = 1.75f;
const float HAMBERT_HEIGHT = 1.5f;
const float HAMBERT_DEPTH = 0.1f;
const float HAMBERT_Z = 0.0f;
const float HAMBERT_MIN_X = -6.0f;
const float HAMBERT_MAX_X = 6.0f;
const const float HAMBERT_SPEED = 10.0f;
const Color HAMBERT_COLOR = BEIGE;

// State
//--------------------------------------------------------------------------------------

typedef enum State
{
    PLAYING,
    GAME_OVER,
} State;

State state = PLAYING;

// Coordinates of panels.
Vector3 Panels[NUM_PANELS];

// X-Coordinate of Hambert.
float Hambert = 0;

//--------------------------------------------------------------------------------------

// Panel Logic
//--------------------------------------------------------------------------------------
void PanelReset(size_t i)
{
    float x = GetRandomValue(PANEL_MIN_X, PANEL_MAX_X);
    Panels[i] = (Vector3){x, 0.0f, PANEL_START_Z};
}

void PanelInit()
{
    for (size_t i = 0; i < NUM_PANELS; i++)
        PanelReset(i);
}

BoundingBox PanelBox(size_t i)
{
    Vector3 p = Panels[i];
    return (BoundingBox){
        (Vector3){p.x - PANEL_WIDTH / 2, p.y - PANEL_HEIGHT / 2, p.z - PANEL_DEPTH / 2},
        (Vector3){p.x + PANEL_WIDTH / 2, p.y + PANEL_HEIGHT / 2, p.z + PANEL_DEPTH / 2},
    };
}

void PanelMove(size_t i, float delta)
{
    Panels[i].z -= PANEL_SPEED * delta;

    if (Panels[i].z < CAMERA_POSITION.z)
        PanelReset(i);
}

void PanelDraw(size_t i)
{
    DrawCube(Panels[i], PANEL_WIDTH, PANEL_HEIGHT, PANEL_DEPTH, PANEL_COLOR);
}

//--------------------------------------------------------------------------------------

// Hambert Logic
//--------------------------------------------------------------------------------------
BoundingBox HambertBox()
{
    return (BoundingBox){
        (Vector3){Hambert - HAMBERT_WIDTH / 2, -HAMBERT_HEIGHT / 2, HAMBERT_Z - HAMBERT_DEPTH / 2},
        (Vector3){Hambert + HAMBERT_WIDTH / 2, HAMBERT_HEIGHT / 2, HAMBERT_Z + HAMBERT_DEPTH / 2}};
}

void HambertMove(bool left, bool right, float delta)
{
    float dx = 0;
    if (right)
        dx -= HAMBERT_SPEED;
    if (left)
        dx += HAMBERT_SPEED;
    Hambert += dx * delta;

    if (Hambert < HAMBERT_MIN_X)
        Hambert = HAMBERT_MIN_X;
    if (Hambert > HAMBERT_MAX_X)
        Hambert = HAMBERT_MAX_X;
}

void HambertDraw()
{
    Vector3 pos = (Vector3){Hambert, 0.0f, HAMBERT_Z};
    DrawCube(pos, HAMBERT_WIDTH, HAMBERT_HEIGHT, HAMBERT_DEPTH, HAMBERT_COLOR);
}
//--------------------------------------------------------------------------------------

// Game Logic
//--------------------------------------------------------------------------------------
bool CheckPanelCollisions()
{
    for (size_t i = 0; i < NUM_PANELS; i++)
        if (CheckCollisionBoxes(PanelBox(i), HambertBox()))
            return true;
    return false;
}

void PlayingUpdate()
{
    float delta = GetFrameTime();
    bool left = IsKeyDown(KEY_A);
    bool right = IsKeyDown(KEY_D);
    HambertMove(left, right, delta);
    for (size_t i = 0; i < NUM_PANELS; i++)
        PanelMove(i, delta);

    if (CheckPanelCollisions())
    {
        state = GAME_OVER;
    }
}
//--------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - core world screen");

    // Define the camera to look into our 3d world
    Camera camera = {0};
    camera.position = CAMERA_POSITION;
    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                         // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;      // Camera projection type
    PanelInit();

    DisableCursor(); // Limit cursor to relative movement inside the window

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------

        switch (state)
        {
        case PLAYING:
            PlayingUpdate();
            break;

        default:
            break;
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        for (size_t i = 0; i < NUM_PANELS; i++)
            PanelDraw(i);

        HambertDraw();

        DrawGrid(10, 1.0f);

        EndMode3D();

        switch (state)
        {
        case PLAYING:
            DrawText("Playing", 10, 10, 40, DARKGRAY);
            break;

        case GAME_OVER:
            DrawText("Game Over", 10, 10, 40, DARKGRAY);
            break;
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}