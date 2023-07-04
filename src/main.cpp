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
move all into separate functions (e.g. findGridIntersect)
*/

using namespace std::chrono_literals;

#define MAX_WIDTH 120
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
    uint8_t map[mapHeight][mapWidth] = {
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
    constexpr float playerSpeed = 0.05f;
    // start pos / angle
    float playerX = 5;
    float playerY = 5;
    float playerAngle = 0;

    int totalFrames = 0;
    float planeX, planeY;
    float currAngle;
    float lineEndX, lineEndY;
    int squareX, squareY;
    float intersectX, intersectY;
    // game loop
    while(true) {
        if(getKeyPressed(XK_q)) {
            break;
        }
        auto startTime = std::chrono::system_clock::now();

        std::system("clear"); // bad fix later
        std::cout << "\x1b[2J\x1b[H\x1b[31m"; 

        // player movement
        if(getKeyPressed(XK_w) && !getKeyPressed(XK_s)) {
            playerX += playerSpeed * cos(playerAngle);
            playerY += playerSpeed * sin(playerAngle);
        } else if(getKeyPressed(XK_s) && !getKeyPressed(XK_w)) {
            playerX -= playerSpeed * cos(playerAngle);
            playerY -= playerSpeed * sin(playerAngle);
        }

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

        std::cout << "angle: " << playerAngle * (180/M_PI) << " x: " << playerX << " y: " << playerY << "\n";

        // raycast to calculate distance values 
        for(int i=0; i <= scrWidth; i++) {
            // current angle will go from leftmost of fov to rightmost in steps of fov/scrwidth
            currAngle = (playerAngle - (fov / 2)) + (i * (fov/scrWidth));

            // actual points on screen viewing plane
            planeX = playerX + (tan(playerAngle - currAngle) * sin(playerAngle)) + cos(playerAngle);
            planeY = playerY + (tan(playerAngle - currAngle) * cos(playerAngle)) + sin(playerAngle);

            lineEndX = playerX + rayLength * cos(currAngle);           
            lineEndY = playerY + rayLength * sin(currAngle);

            // find which grid cell it intersects with
            std::tie(squareX, squareY) = findGridIntersect(playerX, playerY, lineEndX, lineEndY, map, mapHeight, mapWidth);

            // then get exact intersect point
            std::tie(intersectX, intersectY) = raySquareIntersect(
                                                    playerX, playerY, 
                                                    lineEndX, lineEndY, 
                                                    squareX, squareY
                                                );

            std::cout << "x: " << intersectX << " y: " << intersectY << "\n";

            // if no intersection found
            if(std::isnan(intersectX) || std::isnan(intersectY)) {
                std::cout << "NaN intersection\n";
            }
            // store distance to intersection
            distances[i] = sqrt(pow((intersectX - planeX), 2) + 
                                pow((intersectY - planeY), 2));
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
                    line.append("|");
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
    
    std::cout << "\x1b[0m";
    return 0;
}