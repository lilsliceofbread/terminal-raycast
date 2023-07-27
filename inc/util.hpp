#pragma once

struct Vec2f {
    float x;
    float y;
};

void rotate2DVector(Vec2f& vec, float angle);
void moveIfNoCollision(Vec2f& vec, float dx, float dy, uint8_t* map, int mapWidth, int mapHeight);