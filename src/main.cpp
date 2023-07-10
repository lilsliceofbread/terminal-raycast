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
column by column drawing to show how it works
*/

using namespace std::chrono_literals;

#define MAX_WIDTH 130
#define MAX_HEIGHT 65
#define PADDING 5
#define FRAME_LENGTH 16.66ms
#define X_WALL 0
#define Y_WALL 1

void playerInput(Vec2<float>& position, Vec2<float>& dirVec, Vec2<float>& planeVec, float& angle, float turnSpeed, float movementSpeed, uint8_t* map, int mapWidth, int mapHeight);
void drawColumnsToScreen(float* distances, uint8_t* wallColours, int scrWidth, int scrHeight, std::string wallChar, std::string ceilChar, std::string floorChar);

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

    constexpr float turnSpeed = 10.0f;
    constexpr float playerSpeed = 30.0f;
    // start pos / angle
    Vec2<float> playerPos;
    playerPos.x = 12.0f, playerPos.y = 12.0f;
    float playerAngle = 0;

    // using vectors for the actual raycast calculations
    Vec2<float> planeVec;
    planeVec.x = 0, planeVec.y = -1.0f; // initial plane vec relative to initial direction

    Vec2<float> dirVec;
    dirVec.x = cos(playerAngle), dirVec.y = sin(playerAngle);

    // can't use char because unicode :(
    std::string wallChar = "|", ceilChar = "`", floorChar = "~";

    std::cout << "Use Unicode characters? [y for yes]" << std::endl;
    std::string answer;
    std::getline(std::cin, answer);

    std::string yes = "Yy";

    if(yes.find(answer) != std::string::npos) { // if answer is matched in yes string
        wallChar = ceilChar = floorChar = "\u2588";
    } // all other options are the default




    auto gameStartTime = std::chrono::system_clock::now();
    float lastDeltaTime = 0.003; // reasonable starting value
    int totalFrames = 0;
    // game loop
    while(true) {
        if(getKeyPressed(XK_q)) {
            break;
        }
        auto frameStart = std::chrono::system_clock::now();

        int systemRet = std::system("clear"); // bad fix later
        // if system returns error exit
        if(systemRet == -1) exit(1);

        std::cout << "\x1b[2J\x1b[H\x1b[0m"; // clear screen w/ ANSI codes


        
        playerInput(playerPos, dirVec, planeVec, playerAngle, turnSpeed * lastDeltaTime, playerSpeed * lastDeltaTime, map, mapWidth, mapHeight);



        std::cout << "angle: " << playerAngle * (180/M_PI) << " x: " << playerPos.x << " y: " << playerPos.y << "\n";
        


        // raycast to calculate distance values 
        Vec2<float> rayVec;
        Vec2<int> tilePos;
        float currDistX, currDistY;     // distance from player to current point in loop
        float xStepRatio, yStepRatio;
        int xStep, yStep;               // -1 or 1 depending on direction
        for(int i=0; i <= scrWidth; i++) {
            float planeScalar = (2.0f * i / scrWidth) - 1.0f;   // current point along viewing plane from -1 to 1

            rayVec.x = dirVec.x + (planeVec.x * planeScalar);   // ray vec will sweep along viewing plane
            rayVec.y = dirVec.y + (planeVec.y * planeScalar);   // as loop progresses                             

            tilePos.x = floor(playerPos.x), tilePos.y = floor(playerPos.y); // tile of the map player is in

            // length of ray vector when w component is 1 (roughly), same thing for yStepRatio 
            xStepRatio = (rayVec.x == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.x); // if will divide by zero, set infinity 
            yStepRatio = (rayVec.y == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.y); 

            // setting stepping values and starting pos for currDist

            if(rayVec.x < 0) { // x component in -ve direction
                xStep = -1;
                // length along ray vec to get to next gridline by multiplying with ratio
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
            // on lodev.org
            if(wallSide == X_WALL) {
                distances[i] = currDistX - xStepRatio;
                // also store wall colours
                wallColours[i] = map[(tilePos.y * mapHeight) + tilePos.x];
            } else {
                distances[i] = currDistY - yStepRatio;
                wallColours[i] = map[(tilePos.y * mapHeight) + tilePos.x] + 10; // different wall colours for Y_WALLs
            }
        }



        drawColumnsToScreen(distances, wallColours, scrWidth, scrHeight, wallChar, ceilChar, floorChar);



        totalFrames++;
        // can remove later, for debugging
        auto frameEnd = std::chrono::system_clock::now();
        std::chrono::duration<float> frameTime = frameEnd - frameStart;
        std::cout << "\x1b[37mtotal frames: " << totalFrames << "\nframe time: " << frameTime.count() * 1000 << "ms\n";

        lastDeltaTime = frameTime.count();

        // wait until end of frame (frame length - time already run)
        std::this_thread::sleep_for(FRAME_LENGTH - frameTime); 
    }

    std::chrono::duration<float> totalTime = (std::chrono::system_clock::now()) - gameStartTime;
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

void drawColumnsToScreen(float* distances, uint8_t* wallColours, int scrWidth, int scrHeight, std::string wallChar, std::string ceilChar, std::string floorChar) {
    std::string scrBuffer;

    std::string floorColour = "\x1b[32m"; // green
    std::string ceilColour = "\x1b[36m"; // cyan
    std::string ANSIprefix = "\x1b[";

    int currLine = 0;
    float distFromCentre;
    float halfColumnHeight;

    uint8_t prevColour = 0, currColour; // used to determine if to switch colour
    bool prevWasFloorCeil = false;

    for(int i=0; i <= scrHeight; i++) {
        // should get dist to centre of screen, might be off by 1 because im dumb
        distFromCentre = (scrHeight / 2.0f) - currLine; // non absolute
        //std::cout << distFromCentre << "\n";
        for(int j=0; j <= scrWidth; j++) {
            currColour = wallColours[j];

            // inefficient, has to calculate more than necessary
            // precalculating would instead be faster
            halfColumnHeight = scrHeight / distances[j];

            //append character if column is there
            if(abs(distFromCentre) <= halfColumnHeight) { // wall
                // only add ansi colour when necessary
                if(currColour != prevColour || prevWasFloorCeil) {
                    int ANSIcolour = ANSIColourFromColour(currColour);

                    std::string colourStr = ANSIprefix + std::to_string(ANSIcolour) + "m"; 
                    scrBuffer.append(colourStr);
                }
                    
                scrBuffer.append(wallChar);
                prevWasFloorCeil = false;
            } else { // not wall
                if(distFromCentre > 0) { // ceiling
                    // only add ansi colour when necessary
                    if(!prevWasFloorCeil)
                        scrBuffer.append(ceilColour);

                    scrBuffer.append(ceilChar);
                    prevWasFloorCeil = true;
                } else { // floor
                    // only add ansi colour when necessary
                    if(!prevWasFloorCeil)
                        scrBuffer.append(floorColour);

                    scrBuffer.append(floorChar);
                    prevWasFloorCeil = true;
                }
            }
            prevColour = currColour;
        }
        scrBuffer.append("\n");
        currLine++;
    }

    std::cout << scrBuffer;
}