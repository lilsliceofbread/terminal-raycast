#include <sys/ioctl.h>
#include <unistd.h>
#include <tuple>
#include <limits>
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

// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
std::tuple<float, float> lineLineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    float intersectX, intersectY;

    const float denominator = ((x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4));
    if(denominator == 0) { // idk if !denominator would work here, might be slightly faster
        const float nan = std::numeric_limits<float>::quiet_NaN();
        return {nan, nan}; // im using NaN to signify no intersection 
    }

    const float xNumerator = (((x1*y2) - (y1*x2)) * (x3 - x4)) - ((x1 - x2) * ((x3*y4) - (y3*x4)));
    const float yNumerator = (((x1*y2) - (y1*x2)) * (y3 - y4)) - ((y1 - y2) * ((x3*y4) - (y3*x4)));
    
    intersectX = xNumerator / denominator;
    intersectY = yNumerator / denominator;

    return std::make_tuple(intersectX, intersectY);
}

// how the fuck did i come up with this
std::tuple<float, float> lineSquareIntersect(float x1, float y1, float x2, float y2, int squareX, int squareY) {
    float intersectX, intersectY;
    // square point will be centre, 0.5 dist surrounding it
    //const float topLeftX = squareX - 0.5f, topLeftY = squareY - 0.5f;
    //const float topRightX = squareX + 0.5f, topRightY = squareY - 0.5f;
    //const float bottomLeftX = squareX - 0.5f, bottomLeftY = squareY + 0.5f;
    //const float bottomRightX = squareX + 0.5f, bottomRightY = squareY + 0.5f;

    // instead use array as then we can use a loop which is better
    const float squarePoints[8] =  {        // must organise elements so that we can loop through them
        (squareX - 0.5f), (squareY - 0.5f), // top left
        (squareX + 0.5f), (squareY - 0.5f), // top right
        (squareX + 0.5f), (squareY + 0.5f), // bottom right
        (squareX - 0.5f), (squareY + 0.5f)  // bottom left
    };                                      // then back to top left

    int index1, index2;
    float edgeX1, edgeY1, edgeX2, edgeY2;
    for(int i=0; i < 4; i++) {
        index1 = 2 * i;
        index2 = index1 + 2;
        // allow to wrap back around for left edge
        if(index2 >= 8)
            index2 = 0;

        edgeX1 = squarePoints[index1];
        edgeY1 = squarePoints[index1+1];
        edgeX2 = squarePoints[index2];
        edgeY2 = squarePoints[index2+1]; 

        // thought i could use structured bindings but apparently not
        std::tie(intersectX, intersectY) = lineLineIntersect(x1, y1, x2, y2, edgeX1, edgeY1, edgeX2, edgeY2);

        if(!std::isnan(intersectX) && !std::isnan(intersectY)) // we have found the intersection
            break;
    }

    // will return NaN if no intersects found for some reason
    return std::make_tuple(intersectX, intersectY);
}