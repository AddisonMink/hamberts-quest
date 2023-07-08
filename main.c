#include "raylib.h"
#include "stddef.h"
#include "panels.h"

const Vector3 CAMERA_POSITION = {0.0f, 2.5f, -10.0f};

const float HAMBERT_WIDTH = 1.75f;
const float HAMBERT_HEIGHT = 1.5f;
const float HAMBERT_DEPTH = 0.1f;
const float HAMBERT_Z = 0.0f;
const float HAMBERT_MIN_X = -6.0f;
const float HAMBERT_MAX_X = 6.0f;
const float HAMBERT_SPEED = 10.0f;
const Color HAMBERT_COLOR = BEIGE;

// State
//--------------------------------------------------------------------------------------

typedef enum State
{
    PLAYING,
    GAME_OVER,
} State;

State state = PLAYING;

// X-Coordinate of Hambert.
float Hambert = 0;

//--------------------------------------------------------------------------------------

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

void PlayingUpdate(Panels *panels)
{
    float delta = GetFrameTime();
    bool left = IsKeyDown(KEY_A);
    bool right = IsKeyDown(KEY_D);
    HambertMove(left, right, delta);
    PanelsMove(panels, delta);

    if (PanelsCheckCollision(panels, HambertBox()))
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
    Panels *panels = PanelsInit();

    // Define the camera to look into our 3d world
    Camera camera = {0};
    camera.position = CAMERA_POSITION;
    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                         // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;      // Camera projection type

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
            PlayingUpdate(panels);
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
        PanelsDraw(panels);
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