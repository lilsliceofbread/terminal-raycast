#pragma once
#include <X11/Xlib.h>
#include "X11/keysym.h"

void getWinSize(int* w, int* h);
bool getKeyPressed(KeySym key);
std::tuple<float, float> raySquareIntersect(float x1, float y1, float x2, float y2, int squareX, int squareY);

// do this to allow passing reference to 2D map array (Map becomes uint8_t[][])
// scuffed but works decent
// has to be in header file because templates
template <typename Map>
std::tuple<int, int> findGridIntersect(float x1, float y1, float x2, float y2, const Map& map, int mapHeight, int mapWidth) {
    int squareX, squareY;

    float dx = x2 - x1;
    float dy = y2 - y1;

    float step = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    dx = dx / step;
    dy = dy / step;

    float currX = x1;
    float currY = y1;

    for(int j=1; j <= step; j++) {
        // may remove
        squareX = round(currX);
        squareY = round(currY);
        // if current square is a wall, >0 in case i add colours later
        if(squareX > mapHeight - 1 || squareX < 0 || squareY > mapWidth - 1 || squareY < 0)
            return std::make_tuple(squareX, squareY); // if going outside map, return, preventing reading outside map array

        if(map[squareX][squareY] > 0)
            return std::make_tuple(squareX, squareY);
        

        currX += dx;
        currY += dy;
    }
    // for testing purposes remove later
    std::cout << "did not find grid intersection\n";
    exit(1);
}
