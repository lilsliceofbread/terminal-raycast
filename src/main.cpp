#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <thread>
#include <tuple>
#include <limits>
#include "util.hpp"

/*
TODO:
make cross-platform using ncurses
move all into separate functions
*/

using namespace std::chrono_literals;

#define MAX_WIDTH 130
#define MAX_HEIGHT 65
#define PADDING 5
#define FRAME_LENGTH 16.66ms
#define X_WALL 0
#define Y_WALL 1

void playerInput(Vec2<float>& position, Vec2<float>& dirVec, Vec2<float>& planeVec, float& angle, float turnSpeed, float movementSpeed, uint8_t* map, int mapWidth, int mapHeight);

int main() {
    std::cout << 
    " _____                   _             _  ______                          _            \n"
    "|_   _|                 (_)           | | | ___ \\                        | |           \n"
    "  | | ___ _ __ _ __ ___  _ _ __   __ _| | | |_/ /__ _ _   _  ___ __ _ ___| |_ ___ _ __ \n"
    "  | |/ _ \\ '__| '_ ` _ \\| | '_ \\ / _` | | |    // _` | | | |/ __/ _` / __| __/ _ \\ '__|\n"
    "  | |  __/ |  | | | | | | | | | | (_| | | | |\\ \\ (_| | |_| | (_| (_| \\__ \\ ||  __/ |   \n"
    "  \\_/\\___|_|  |_| |_| |_|_|_| |_|\\__,_|_| \\_| \\_\\__,_|\\__, |\\___\\__,_|___/\\__\\___|_|   \n"
    "                                                       __/ |                           \n"
    "                                                      |___/                            \n"
    << std::endl;


    int winWidth, winHeight;
    getWinSize(&winWidth, &winHeight);

    // ensure "screen" size is not greater than terminal window size
    const int scrWidth = (winWidth - PADDING < MAX_WIDTH) ? winWidth - PADDING : MAX_WIDTH;
    const int scrHeight = (winHeight - PADDING < MAX_HEIGHT) ? winHeight - PADDING : MAX_HEIGHT;
   
    float distances[scrWidth] = {0}; // will store distances from player to surrounds
    uint8_t wallColours[scrWidth] = {0}; // colours of walls
    std::string screen[scrHeight];

    // 1 for wall, 0 for air
    constexpr int mapWidth = 25;
    constexpr int mapHeight = 25;
    // not const incase i want to update it at runtime
    uint8_t map[mapWidth * mapHeight] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,3,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,3,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,3,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,4,4,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,4,4,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    };

    constexpr float turnSpeed = 0.02f;
    constexpr float playerSpeed = 0.12f;
    // start pos / angle
    Vec2<float> playerPos;
    playerPos.x = 12.0f, playerPos.y = 12.0f;
    float playerAngle = 0;

    // using vectors for the actual raycast calculations
    Vec2<float> planeVec;
    planeVec.x = 0, planeVec.y = -1.0f; // initial plane vec relative to initial direction

    Vec2<float> dirVec;
    dirVec.x = cos(playerAngle), dirVec.y = sin(playerAngle);

    // can't use char because unicode requires more bytes
    std::string wallChar = "|", ceilChar = "`", floorChar = "~";

    std::cout << "Use Unicode characters? [y for yes]" << std::endl;
    std::string answer;
    std::getline(std::cin, answer);

    std::string yes = "Yy";

    if(yes.find(answer) != std::string::npos) { // if answer is matched in yes string
        wallChar = ceilChar = floorChar = "\u2588";
    } // all other options are the default



    auto gameStartTime = std::chrono::system_clock::now();
    int totalFrames = 0;
    // game loop
    while(true) {
        if(getKeyPressed(XK_q)) {
            break;
        }
        auto frameStart = std::chrono::system_clock::now();

        std::system("clear"); // bad fix later
        std::cout << "\x1b[2J\x1b[H\x1b[0m"; 

        //player movement
        playerInput(playerPos, dirVec, planeVec, playerAngle, turnSpeed, playerSpeed, map, mapWidth, mapHeight);

        std::cout << "angle: " << playerAngle * (180/M_PI) << " x: " << playerPos.x << " y: " << playerPos.y << "\n";
        std::cout << "dir vec: " << dirVec.x << ", " << dirVec.y << " plane vec: " << planeVec.x << ", " << planeVec.y << "\n";
        


        // raycast to calculate distance values 
        for(int i=0; i <= scrWidth; i++) {
            float planeScalar = (2.0f * i / scrWidth) - 1.0f;   // current point along viewing plane from -1 to 1

            Vec2<float> rayVec;
            rayVec.x = dirVec.x + (planeVec.x * planeScalar);// ray vec will sweep along viewing plane
            rayVec.y = dirVec.y + (planeVec.y * planeScalar);// as loop progresses                             
            //std::cout << "Ray Vector x: " << rayVec.x << " y: " << rayVec.y << "\n";

            Vec2<int> tilePos;
            tilePos.x = floor(playerPos.x), tilePos.y = floor(playerPos.y); // tile of the map player is in

            float currDistX, currDistY;                         // distance from player to current point in loop
            //std::cout << "Tile x: " << tilePos.x << " y: " << tilePos.y << "\n";
            
            // length of ray vector when w component is 1 (roughly), same thing for yStepRatio 
            float xStepRatio = (rayVec.x == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.x); // if will divide by zero, set infinity 
            float yStepRatio = (rayVec.y == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.y); 
            //std::cout << "StepRatio x: " << xStepRatio << " y: " << yStepRatio << "\n";
        
            // -1 or 1 depending on direction
            int xStep, yStep;

            // setting stepping values and starting pos for currDist

            if(rayVec.x < 0) { // x component in -ve direction
                xStep = -1;
                // length along ray vec to get to next gridline
                currDistX = (playerPos.x - tilePos.x) * xStepRatio;
            } else { // x component in +ve direction
                xStep = 1;
                // this time get x distance from tile to right, then multiply with ratio
                currDistX = ((tilePos.x + 1.0f) - playerPos.x) * xStepRatio;
            }
            // same for y
            if(rayVec.y < 0) { // y component in -ve direction
                yStep = -1;
                currDistY = (playerPos.y - tilePos.y) * yStepRatio;
            } else { // y component in +ve direction
                yStep = 1;
                currDistY = ((tilePos.y + 1.0f) - playerPos.y) * yStepRatio;
            }

            uint8_t wallSide;
            bool hitWall = false;
            while(!hitWall) {
                // whichever value is the smallest will be increased
                if(currDistX < currDistY) {
                    // move along to next intersection with gridlines
                    currDistX += xStepRatio;
                    // increment tiled location
                    tilePos.x += xStep;
                    // if collision is found, it will be with an x facing wall
                    wallSide = X_WALL; 
                } else {
                    currDistY += yStepRatio;

                    tilePos.y += yStep;

                    wallSide = Y_WALL;
                }

                // found an intersection
                if(map[(tilePos.y * mapHeight) + tilePos.x] > 0)
                    hitWall = true;
            }

            // get distance to wall
            // this is derived using similar triangles
            // by other people smarter than me (lodev.org)
            if(wallSide == X_WALL) {
                distances[i] = currDistX - xStepRatio;
                // also store wall colours
                wallColours[i] = map[(tilePos.y * mapHeight) + tilePos.x];
                //std::cout << "x: " << currDistX << ", " << xStepRatio << " dist: " << distances[i] << "\n";
            } else {
                distances[i] = currDistY - yStepRatio;
                wallColours[i] = map[(tilePos.y * mapHeight) + tilePos.x] + 10; // different wall colours for Y_WALLs
                //std::cout << "y: " << currDistY << ", " << yStepRatio << " dist: " << distances[i] << "\n";
            }
        }



        // draw columns to string
        // doing this intermediate step means drawing each line to screen is done concurrently
        // move this into function!!!
        int currLine = 0;
        float distFromCentre;
        float halfColumnHeight;
        uint8_t prevColour = 0, currColour; // used to determine if to switch colour
        std::string floorColour = "\x1b[37m"; // white
        std::string ceilColour = "\x1b[36m"; // cyan
        bool prevWasFloorCeil = false;
        for(std::string& line : screen) {
            line = ""; // remove previous line
            // should get dist to centre of screen, might be off by 1
            distFromCentre = (scrHeight / 2.0f) - currLine; // non absolute
            //std::cout << distFromCentre << "\n";
            for(int i=0; i <= scrWidth; i++) {
                currColour = wallColours[i];

                // inefficient, has to calculate more than necessary
                // precalculating would instead be faster
                halfColumnHeight = scrHeight / (1.0f * distances[i]);

                //append character if column is there
                if(abs(distFromCentre) <= halfColumnHeight) {
                    // only add ansi colour when necessary
                    if(currColour != prevColour || prevWasFloorCeil) {
                        line.append(ANSIStringFromColour(currColour));
                    }
                    
                    line.append(wallChar); // wall
                    prevWasFloorCeil = false;
                } else {
                    if(distFromCentre > 0) {
                        // only add ansi colour when necessary
                        if(!prevWasFloorCeil)
                            line.append(ceilColour);

                        line.append(ceilChar); // ceiling
                        prevWasFloorCeil = true;
                    } else {
                        // only add ansi colour when necessary
                        if(!prevWasFloorCeil)
                            line.append(floorColour);

                        line.append(floorChar); // floor
                        prevWasFloorCeil = true;
                    }
                }
                prevColour = currColour;
            }
            currLine++;
        }



        // actually print lines to screen
        for (const std::string& line : screen) {
            // i think std::endl is slow as it also clears the buffer
            std::cout << line << "\n";
        }



        totalFrames++;
        // can remove later, for debugging
        auto frameEnd = std::chrono::system_clock::now();
        std::chrono::duration<double> frameTime = frameEnd - frameStart;
        std::cout << "\x1b[37mtotal frames: " << totalFrames << "\nframe time: " << frameTime.count() * 1000 << "ms\n";

        // wait until end of frame (frame length - time already run)
        std::this_thread::sleep_for(FRAME_LENGTH - frameTime); 
    }

    std::chrono::duration<double> totalTime = (std::chrono::system_clock::now()) - gameStartTime;
    std::cout << "\x1b[0mtotal time running: " << totalTime.count() << "s\n";
    
    return 0;
}

void playerInput(Vec2<float>& position, Vec2<float>& dirVec, Vec2<float>& planeVec, float& angle, float turnSpeed, float movementSpeed, uint8_t* map, int mapWidth, int mapHeight) {
        // player movement
        if(getKeyPressed(XK_Right)) {
            angle -= turnSpeed;

            // must recalculate vectors
            dirVec.x = cos(angle); // dir vec is just unit vector
            dirVec.y = sin(angle); // so is just cos and sin

            rotate2DVector(planeVec, -turnSpeed);
        } else if (getKeyPressed(XK_Left)) {
            angle += turnSpeed;
            
            dirVec.x = cos(angle); 
            dirVec.y = sin(angle);

            rotate2DVector(planeVec, turnSpeed);
        }

        // prevent angle > 360 or < 0
        if(angle > (2.0f * M_PI)) {
            angle -= 2.0f * M_PI;
        } else if (angle < 0) {
            angle += 2.0f * M_PI;
        }

        if(getKeyPressed(XK_w) && !getKeyPressed(XK_s)) {
            moveIfNoCollision(position, movementSpeed * dirVec.x, movementSpeed * dirVec.y, map, mapWidth, mapHeight);
        } else if(getKeyPressed(XK_s) && !getKeyPressed(XK_w)) {
            moveIfNoCollision(position, -(movementSpeed * dirVec.x), -(movementSpeed * dirVec.y), map, mapWidth, mapHeight);
        }

        // perpendicular vector is sin and -cos or -sin and cos
        if(getKeyPressed(XK_a) && !getKeyPressed(XK_d)) {
            moveIfNoCollision(position, -(movementSpeed * dirVec.y), movementSpeed * dirVec.x, map, mapWidth, mapHeight);
        } else if(getKeyPressed(XK_d) && !getKeyPressed(XK_a)) {
            moveIfNoCollision(position, movementSpeed * dirVec.y, -(movementSpeed * dirVec.x), map, mapWidth, mapHeight);
        } 
}