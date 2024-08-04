#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include "util.hpp"

void rotate2DVector(Vec2f& vec, float angle)
{
    const float oldVecX = vec.x;
    // rotate using 2d rotation matrix
    vec.x = (vec.x * std::cos(angle)) - (vec.y * std::sin(angle));
    vec.y = (oldVecX * std::sin(angle)) + (vec.y * std::cos(angle));
}

void moveIfNoCollision(Vec2f& vec, float dx, float dy, uint8_t* map, int mapWidth, int mapHeight)
{
    float newX = vec.x + dx, newY = vec.y + dy;

    // conditions separated for clarity
    if(newX < 0 || newX > mapWidth || newY < 0 || newY > mapHeight)
    { // if outside map 
        return;
    }
    else if(map[(int(newY) * mapWidth) + int(newX)] > 0)
    { // if colliding with block
        return;
    }

    vec.x = newX;
    vec.y = newY;
}