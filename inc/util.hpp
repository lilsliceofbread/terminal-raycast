#pragma once
#include <X11/Xlib.h>
#include "X11/keysym.h"

void getWinSize(int* w, int* h);
bool getKeyPressed(KeySym key);
std::tuple<float, float> lineSquareIntersect(float x1, float y1, float x2, float y2, int squareX, int squareY);