#include "panels.h"
#include "stddef.h"

const int NUM_PANELS = 2;
const float PANEL_WIDTH = 2.0f;
const float PANEL_HEIGHT = 2.0f;
const float PANEL_DEPTH = 0.1f;
const float PANEL_SPEED = 20.0f;
const int PANEL_START_Z = 20;
const int PANEL_MIN_X = 4;
const int PANEL_MAX_X = -6;
const Color PANEL_COLOR = RED;

typedef struct Panels
{
    Vector3 panels[NUM_PANELS];
} Panels;

void panelsReset(Panels *panels)
{
    for (size_t i = 0; i < NUM_PANELS; i++)
    {
        float x = GetRandomValue(PANEL_MIN_X, PANEL_MAX_X);
        panels->panels[i] = (Vector3){x, 0.0f, PANEL_START_Z};
    }
}

Panels *PanelsInit()
{
    Panels *panels = MemAlloc(sizeof(Panels));
    panelsReset(panels);
    return panels;
}

void PanelsFree(Panels *panels)
{
    MemFree(panels);
}

void PanelsMove(Panels *panels, float delta)
{
    for (size_t i = 0; i < NUM_PANELS; i++)
    {
        Vector3 *pos = &panels->panels[i];
        pos->z -= PANEL_SPEED * delta;
        if (pos->z < -10.0f)
        {
            panelsReset(panels);
            break;
        }
    }
}

bool PanelsCheckCollision(Panels *panels, BoundingBox box)
{
    for (size_t i = 0; i < NUM_PANELS; i++)
    {
        Vector3 p = panels->panels[i];
        BoundingBox panelBox = (BoundingBox){
            (Vector3){p.x - PANEL_WIDTH / 2, p.y - PANEL_HEIGHT / 2, p.z - PANEL_DEPTH / 2},
            (Vector3){p.x + PANEL_WIDTH / 2, p.y + PANEL_HEIGHT / 2, p.z + PANEL_DEPTH / 2},
        };
        if (CheckCollisionBoxes(box, panelBox))
            return true;
    }
    return false;
}

void PanelsDraw(Panels *panels)
{
    for (size_t i = 0; i < NUM_PANELS; i++)
    {
        DrawCube(panels->panels[i], PANEL_WIDTH, PANEL_HEIGHT, PANEL_DEPTH, PANEL_COLOR);
    }
}