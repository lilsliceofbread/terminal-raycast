#include <sys/ioctl.h>
#include <unistd.h>
#include <tuple>
#include <iostream>
#include <cmath>
#include "util.hpp"

void getWinSize(int* w, int* h) {
    winsize terminal_size;

    // linux function to query kernel stuff
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);

    *w = terminal_size.ws_col;
    *h = terminal_size.ws_row;
}

bool getKeyPressed(KeySym key) {
    Display *display = XOpenDisplay(":0");
    char keysReturn[32]; // 32*8 = 256 bits for ascii codes

    XQueryKeymap(display, keysReturn);
    // gets actual ascii keycode - KeyCode is just unsigned char
    KeyCode keycode = XKeysymToKeycode(display, key); 

    // scuffed bitwise ops to get if bitflag of keyReturn is pressed using keycode
    // https://stackoverflow.com/questions/18281412/check-keypress-in-c-on-linux
    bool isPressed = (keysReturn[keycode >> 3] & (1 << (keycode & 7)));

    XCloseDisplay(display);
    return isPressed;
}

int ANSIColourFromColour(uint8_t colour) {
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
            ANSIcolour = 34; // blue
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
            ANSIcolour = 94; // bright blue
            break;
        case 14:
            ANSIcolour = 95; // bright magenta
            break;
    }

    return ANSIcolour;
}

void rotate2DVector(Vec2<float>& vec, float angle) {
    const float oldVecX = vec.x;
    // rotate using 2d rotation matrix
    vec.x = (vec.x * cos(angle)) - (vec.y * sin(angle));
    vec.y = (oldVecX * sin(angle)) + (vec.y * cos(angle));
}

void moveIfNoCollision(Vec2<float>& vec, float dx, float dy, uint8_t* map, int mapWidth, int mapHeight) {
    float newX = vec.x + dx, newY = vec.y + dy;

    if(map[(int(newY) * mapWidth) + int(newX)] > 0) {
        return;
    }

    vec.x = newX;
    vec.y = newY;
}