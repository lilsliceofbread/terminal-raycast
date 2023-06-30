#pragma once
#include <X11/Xlib.h>
#include "X11/keysym.h"

void getWinSize(int* w, int* h);
bool getKeyPressed(KeySym key);