//#include <sys/ioctl.h>
//#include <unistd.h>
#include <iostream>
#include <cmath>
#include "util.hpp"

/*bool getKeyPressed(int keyCode) {
    int keyPressed = getch();
    if(keyPressed == ERR) return false; //remove ltr
    //printw("%d\n", keyPressed);
    return keyPressed == keyCode; 
}*/

/*int ANSIColourFromColour(uint8_t colour) {
    int ANSIcolour = 0;

    // shouldn't get 0 as it is not a wall
    switch(colour) {
        // darker X_WALLs
        case 1:
            ANSIcolour = 37; // white background walls 
            break;
        case 2:
            ANSIcolour = 31; // red
            break;
        case 3:
            ANSIcolour = 33; // yellow
            break;
        case 4:
            ANSIcolour = 35; // magenta
            break;
        // bright Y_WALLs
        case 11:
            ANSIcolour = 97; // bright white
            break;
        case 12:
            ANSIcolour = 91; // bright red
            break;
        case 13:
            ANSIcolour = 93; // bright yellow
            break;
        case 14:
            ANSIcolour = 95; // bright magenta
            break;
    }

    return ANSIcolour;
}*/

void rotate2DVector(Vec2f& vec, float angle) {
    const float oldVecX = vec.x;
    // rotate using 2d rotation matrix
    vec.x = (vec.x * std::cos(angle)) - (vec.y * std::sin(angle));
    vec.y = (oldVecX * std::sin(angle)) + (vec.y * std::cos(angle));
}

void moveIfNoCollision(Vec2f& vec, float dx, float dy, uint8_t* map, int mapWidth, int mapHeight) {
    float newX = vec.x + dx, newY = vec.y + dy;

    // conditions separated for clarity
    if(newX < 0 || newX > mapWidth || newY < 0 || newY > mapHeight) { // if outside map 
        return;
    } else if(map[(int(newY) * mapWidth) + int(newX)] > 0) { // if colliding with block
        return;
    }

    vec.x = newX;
    vec.y = newY;
}