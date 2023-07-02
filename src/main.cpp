#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <thread>
#include <tuple>
#include "util.hpp"

/*
TODO:
make cross-platform using ncurses
*/

using namespace std::chrono_literals;

#define MAX_WIDTH 70
#define MAX_HEIGHT 50
#define FRAME_LENGTH 16.66ms

int main() {
    int winWidth, winHeight;
    getWinSize(&winWidth, &winHeight);

    // ensure "screen" size is not greater than terminal screen size
    const int scrWidth = (winWidth < MAX_WIDTH) ? winWidth : MAX_WIDTH;
    const int scrHeight = (winHeight < MAX_HEIGHT) ? winHeight : MAX_HEIGHT;
   
    float distances[scrWidth] = {0}; // will store distances from player to surrounds
    std::string screen[scrHeight];
    // 1 for wall, 0 for air
    constexpr int mapWidth = 10;
    constexpr int mapHeight = 10;
    uint8_t map[mapWidth][mapHeight] = {
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

    const float rayLength = sqrt(mapWidth*mapWidth + mapHeight*mapHeight);

    constexpr float fov = M_PI_4; // 45 deg field of view
    constexpr float turnSpeed = 0.015f;
    // start pos / angle
    int playerX = 5;
    int playerY = 5;
    float playerAngle = 0;

    int totalFrames = 0;
    float currAngle;
    float lineEndX, lineEndY;
    float dxLine,dyLine; // variables for DDA line
    float step;
    float currX, currY;
    int squareX, squareY;
    float intersectX, intersectY;
    // game loop
    while(true) {
        if(getKeyPressed(XK_q)) {
            break;
        }
        auto startTime = std::chrono::system_clock::now();

        std::system("clear"); // bad fix later
        std::cout << "\x1b[2J\x1b[H"; 

        // player movement
        if(getKeyPressed(XK_Right)) {
            playerAngle += turnSpeed;
        } else if (getKeyPressed(XK_Left)) {
            playerAngle -= turnSpeed;
        }

        // prevent angle > 360 or < 0
        if(playerAngle > (2 * M_PI)) {
            playerAngle -= 2 * M_PI;
        } else if (playerAngle < 0) {
            playerAngle += 2 * M_PI;
        }

        std::cout << "playerAngle: " << playerAngle * (180/M_PI) << " deg\n";

        // raycast to calculate distance values 
        for(int i=0; i <= scrWidth; i++) {
            // current angle will go from leftmost of fov to rightmost in steps of fov/scrwidth
            currAngle = (playerAngle - (fov / 2)) + (i * (fov/scrWidth));

            // DDA algorithm to find intersection with block

            lineEndX = playerX + (rayLength * cos(currAngle));           
            lineEndY = playerY + (rayLength * sin(currAngle));

            // dx = end - start = x + LENcos(angle) - x = LENcos(angle)
            dxLine = lineEndX - playerX;
            dyLine = lineEndY - playerY;

            step = (abs(dxLine) >= abs(dyLine)) ? abs(dxLine) : abs(dyLine);

            dxLine = dxLine / step;
            dyLine = dyLine / step;

            currX = playerX;
            currY = playerY;
            for(int j=1; j <= step; j++) {
                // may remove
                squareX = round(currX);
                squareY = round(currY);
                // if current square is a wall
                if(map[squareX][squareY] == 1) {
                    // distance between wall and player, weird constants to correct for distance
                    /*distances[i] = 0.8 + (0.4 * sqrt(
                                        pow((squareX - playerX), 2) + 
                                        pow((squareY - playerY), 2)));*/
                    std::tie(intersectX, intersectY) = lineSquareIntersect(
                                                            playerX, playerY, 
                                                            lineEndX, lineEndY, 
                                                            squareX, squareY
                                                       );
                    //std::cout << "x: " << intersectX << " y: " << intersectY << "\n";
                    distances[i] = sqrt(pow((intersectX - playerX), 2) + 
                                        pow((intersectY - playerY), 2));
                    break;
                }
                currX += dxLine;
                currY += dyLine;
            }
            //std::cout << distances[i] << "\n";
        }

        int currLine = 0.5;
        float distFromCentre;
        float halfColumnHeight;
        // draw columns to string
        // doing this intermediate step means drawing each line to screen is done concurrently
        for(std::string& line : screen) {
            line = ""; // remove previous line
            // should get dist to centre of screen, might be off by 1 on even screen heights
            distFromCentre = abs((scrHeight / 2) - currLine);
            //std::cout << distFromCentre << "\n";
            for(int i=0; i<=scrWidth; i++) {
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