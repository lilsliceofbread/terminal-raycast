#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <thread>
#include "util.hpp"

/*
TODO:
make cross-platform using ncurses
*/

using namespace std::chrono_literals;

#define MAX_WIDTH 50
#define MAX_HEIGHT 35
#define RAY_LENGTH 20
#define FRAME_LENGTH 1000ms

int main() {
    int winWidth, winHeight;
    getWinSize(&winWidth, &winHeight);

    // ensure "screen" size is not greater than terminal screen size
    const int scrWidth = (winWidth < MAX_WIDTH) ? winWidth : MAX_WIDTH;
    const int scrHeight = (winHeight < MAX_HEIGHT) ? winHeight : MAX_HEIGHT;
   
    float distances[scrWidth] = {0}; // will store distances from player to surrounds
    std::string screen[scrHeight];
    // 1 for wall, 0 for air
    uint8_t map[10][10] = {
        {1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1}
    };

    const float fov = M_PI_2; // pi/2 radians field of view
    // start pos / angle
    int playerX = 5;
    int playerY = 5;
    float playerAngle = 0;

    int totalFrames = 0;
    float currAngle;
    float dxLine,dyLine; // variables for DDA line
    float step;
    float currX, currY;
    int squareX, squareY;
    // game loop
    while(true) {
        if(getKeyPressed(XK_q)) {
            break;
        }
        auto startTime = std::chrono::system_clock::now();

        std::system("clear"); // bad fix later
        std::cout << "\x1b[2J\x1b[H"; 

        // player movement
        playerAngle += M_PI_4;
        // prevent angle > 360 (idk if required)
        if(playerAngle > (2 * M_PI))
            playerAngle -= 2 * M_PI;
        std::cout << "playerAngle: " << playerAngle << "\n";

        // raycast to calculate distance values 
        for(int i=0; i <= scrWidth; i++) {
            // current angle will go from leftmost of fov to rightmost in steps of fov/scrwidth
            currAngle = (playerAngle - (fov / 2)) + (i * (fov/scrWidth));

            //std::cout << "currAngle: " << currAngle << "\n";
            // DDA algorithm to find intersection with block

            // dx = end - start = x + LENcos(angle) - x = LENcos(angle)
            dxLine = RAY_LENGTH * cos(currAngle);           
            dyLine = RAY_LENGTH * sin(currAngle);

            //std::cout << "dx " << dxLine << " dy " << dyLine << "\n";

            step = (abs(dxLine) >= abs(dyLine)) ? abs(dxLine) : abs(dyLine);
            //std::cout << step << "\n";

            dxLine = dxLine / step;
            dyLine = dyLine / step;

            currX = playerX;
            currY = playerY;
            for(int j=1; j <= step; j++) {
                // store rounded values as they are used twice
                squareX = round(currX);
                squareY = round(currY);
                // if current square is a wall
                if(map[squareX][squareY] == 1) {
                    //distances[i] = 0.8 + (0.4 * sqrt(squareX*squareX + squareY*squareY));
                    distances[i] = sqrt(squareX*squareX + squareY*squareY);
                    break;
                }
                currX += dxLine;
                currY += dyLine;
            }
            //std::cout << distances[i] << "\n";
        }

        // draw columns to string
        // doing this intermediate step means drawing each line to screen is done concurrently
        int currLine = 0.5;
        float distFromCentre;
        float halfColumnHeight;
        for(std::string& line : screen) {
            line = ""; // remove previous line
            // should get dist to centre of screen, might be off by 1 on even screen heights
            distFromCentre = abs((scrHeight / 2) - currLine);
            //std::cout << distFromCentre << "\n";
            for(int i=0; i<scrWidth; i++) {
                // inefficient, has to calculate more than necessary
                // looping through columns instead would be faster
                halfColumnHeight = scrHeight / (2 * distances[i]);
                //append character if column is there
                if(distFromCentre <= halfColumnHeight) {
                    line.append("#");
                } else {
                    line.append(" ");
                }
            }
            currLine++;
        }

        for (const std::string& line : screen) {
            // i think std::endl is slow?
            std::cout << line << "\n";
        }

        totalFrames++;
        // can remove later, for debugging
        auto endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> deltaTime = endTime - startTime;
        std::cout << "total frames: " << totalFrames << "\nframe time: " << deltaTime.count() << "\n";

        // wait until end of frame (frame length - time already run)
        std::this_thread::sleep_for(FRAME_LENGTH - deltaTime); 
    }
    
    return 0;
}