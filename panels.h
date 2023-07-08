#ifndef PANELS_H
#define PANELS_H

#include "raylib.h"

typedef struct Panels Panels;

Panels *PanelsInit();

void PanelsFree(Panels *panels);

void PanelsMove(Panels *panels, float delta);

bool PanelsCheckCollision(Panels *panels, BoundingBox box);

void PanelsDraw(Panels *panels);

#endif