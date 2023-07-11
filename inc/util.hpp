#pragma once
#include <X11/Xlib.h>
#include "X11/keysym.h"

struct Vec2f {
    float x;
    float y;
};

void rotate2DVector(Vec2f& vec, float angle);
void moveIfNoCollision(Vec2f& vec, float dx, float dy, uint8_t* map, int mapWidth, int mapHeight);

void getWinSize(int* w, int* h);
bool getKeyPressed(KeySym key);
int ANSIColourFromColour(uint8_t colour);