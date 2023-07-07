#pragma once
#include <X11/Xlib.h>
#include "X11/keysym.h"

template <typename T>
struct Vec2 {
    T x;
    T y;
};

void getWinSize(int* w, int* h);
bool getKeyPressed(KeySym key);
std::string ANSIStringFromColour(uint8_t colour);