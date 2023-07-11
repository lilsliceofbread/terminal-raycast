#include <iostream>
#include <cmath>
#include <limits>
#include "util.hpp"
#include "player.hpp"
#include "raycaster.hpp"

#define MAX_WIDTH 130
#define MAX_HEIGHT 65
#define PADDING 5
#define X_WALL 0
#define Y_WALL 1
#define LOOP_LIMIT 1000

Raycaster::Raycaster(uint8_t* map, int mapWidth, int mapHeight, Player* player)
: mPlayerPtr(player), mMapPtr(map), mMapWidth(mapWidth), mMapHeight(mapHeight) {
    int winWidth, winHeight;
    getWinSize(&winWidth, &winHeight);

    // ensure "screen" size is not greater than terminal window size
    mScrWidth = (winWidth - PADDING < MAX_WIDTH) ? winWidth - PADDING : MAX_WIDTH;
    mScrHeight = (winHeight - PADDING < MAX_HEIGHT) ? winHeight - PADDING : MAX_HEIGHT;

    mDistances = new float[mScrWidth];
    mWallColours = new uint8_t[mScrWidth];

    // can't use char because unicode :(
    wallChar = "|";
    ceilChar = "`";
    floorChar = "~";
}

Raycaster::~Raycaster() {
    delete[] mDistances;
    delete[] mWallColours;
}

void Raycaster::SetUnicode() {
    wallChar = ceilChar = floorChar = "\u2588";
}

void Raycaster::Raycast() {
        // raycast to calculate distance values 
        Vec2f rayVec;
        int tileX, tileY;
        float currDistX, currDistY;     // distance from player to current point in loop
        float xStepRatio, yStepRatio;
        int xStep, yStep;               // -1 or 1 depending on direction

        Vec2f dirVec = mPlayerPtr->GetDirVec();
        Vec2f planeVec = mPlayerPtr->GetPlaneVec();
        Vec2f playerPos = mPlayerPtr->GetPos();

        for(int i=0; i < mScrWidth; i++) {
            float planeScalar = (2.0f * i / mScrWidth) - 1.0f;   // current point along viewing plane from -1 to 1

            rayVec.x = dirVec.x + (planeVec.x * planeScalar);   // ray vec will sweep along viewing plane
            rayVec.y = dirVec.y + (planeVec.y * planeScalar);   // as loop progresses                             

            tileX = floor(playerPos.x), tileY = floor(playerPos.y); // tile of the map player is in

            // length of ray vector when w component is 1 (roughly), same thing for yStepRatio 
            xStepRatio = (rayVec.x == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.x); // if will divide by zero, set infinity 
            yStepRatio = (rayVec.y == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.y); 

            // setting stepping values and starting pos for currDist

            if(rayVec.x < 0) { // x component in -ve direction
                xStep = -1;
                // length along ray vec to get to next gridline by multiplying with ratio
                currDistX = (playerPos.x - tileX) * xStepRatio;
            } else { // x component in +ve direction
                xStep = 1;
                // this time get x distance from tile to right, then multiply with ratio
                currDistX = ((tileX + 1.0f) - playerPos.x) * xStepRatio;
            }
            // same for y
            if(rayVec.y < 0) { // y component in -ve direction
                yStep = -1;
                currDistY = (playerPos.y - tileY) * yStepRatio;
            } else { // y component in +ve direction
                yStep = 1;
                currDistY = ((tileY + 1.0f) - playerPos.y) * yStepRatio;
            }

            uint8_t wallSide;
            bool hitWall = false;
            int rayIters = 0; // loop limit incase some infinite loop condition
            while(!hitWall && rayIters < LOOP_LIMIT) {
                // whichever value is the smallest will be increased
                if(currDistX < currDistY) {
                    // move along to next intersection with gridlines
                    currDistX += xStepRatio;
                    // increment tiled location
                    tileX += xStep;
                    // if collision is found, it will be with an x facing wall
                    wallSide = X_WALL; 
                } else {
                    currDistY += yStepRatio;

                    tileY += yStep;

                    wallSide = Y_WALL;
                }

                if(tileX < 0 || tileX > mMapWidth || tileY < 0 || tileY > mMapHeight) {
                    // if outside of map
                    hitWall = true;
                } else if(mMapPtr[(tileY * mMapHeight) + tileX] > 0) {
                    // found an intersection
                    hitWall = true;
                }
                rayIters++;
            }

            // get distance to wall
            // this is derived using similar triangles
            // on lodev.org
            if(wallSide == X_WALL) {
                mDistances[i] = currDistX - xStepRatio;
                // also store wall colours
                mWallColours[i] = mMapPtr[(tileY * mMapHeight) + tileX];
            } else {
                mDistances[i] = currDistY - yStepRatio;
                mWallColours[i] = mMapPtr[(tileY * mMapHeight) + tileX] + 10; // different wall colours for Y_WALLs
            }
        }
}

void Raycaster::Draw() {
    std::string scrBuffer;

    std::string floorColour = "\x1b[32m"; // green
    std::string ceilColour = "\x1b[34m"; // blue
    std::string ANSIprefix = "\x1b[";

    int currLine = 0;
    float distFromCentre;
    float halfColumnHeight;

    uint8_t prevColour = 0, currColour; // used to determine if to switch colour
    bool prevWasFloorCeil = false;

    for(int i=0; i < mScrHeight; i++) {
        // should get dist to centre of screen, might be off by 1 because im dumb
        distFromCentre = (mScrHeight / 2.0f) - currLine; // non absolute
        //std::cout << distFromCentre << "\n";
        for(int j=0; j < mScrWidth; j++) {
            currColour = mWallColours[j];

            // inefficient, has to calculate more than necessary
            // precalculating would instead be faster
            halfColumnHeight = mScrHeight / mDistances[j];

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