#ifdef __linux__
#include <ncurses.h>
#elif _WIN32
#include <curses.h>
#endif

#include <iostream>
#include <cmath>
#include <limits>
#include "util.hpp"
#include "player.hpp"
#include "raycaster.hpp"

#define MAX_WIDTH 150
#define MAX_HEIGHT 70
#define PADDING 4
#define X_WALL 0
#define Y_WALL 1
#define LOOP_LIMIT 1000

Raycaster::Raycaster(uint8_t* map, int mapWidth, int mapHeight, Player* player)
: mPlayerPtr(player), mMapPtr(map), mMapWidth(mapWidth), mMapHeight(mapHeight), mHasInitWindow(false) {
    
    wallChar = '|';
    ceilChar = '`';
    floorChar = '~';
}

Raycaster::~Raycaster() {
    delete[] mDistances;
    delete[] mWallColours;

    // allowing ncurses to give back terminal if window was initialised
    if(mHasInitWindow) {
        delwin(mWindow);
        endwin();
    }
}

void Raycaster::Print(const std::string& str) {
    if(!mHasInitWindow) {
        return;
    }
    printw("%s", str.c_str());
}

void Raycaster::InitWindow() {
    // in case not initialised don't want to delete in destructor
    mHasInitWindow = true;

    initscr();
    cbreak(); // disable input buffering until enter is pressed
    noecho(); // disable input being displayed to terminal
    keypad(stdscr, TRUE); // allow special character input to be detected e.g. arrow keys
    nodelay(stdscr, TRUE); // don't wait for input with getch()
    
    // could center on screen? (winHeight - scrHeight)/2
    mWindow = newwin(mScrHeight + PADDING, mScrWidth, 0, 0);


    // must do this after initscr() call
    int winHeight, winWidth;
    getmaxyx(stdscr, winHeight, winWidth); // ncurses macro, which is why not passing in &h, &w
    if(winWidth < 0 || winHeight < 0) exit(1);

    // ensure "screen" size is not greater than terminal window size
    mScrWidth = (winWidth - PADDING < MAX_WIDTH) ? winWidth - PADDING : MAX_WIDTH;
    mScrHeight = (winHeight - PADDING < MAX_HEIGHT) ? winHeight - PADDING : MAX_HEIGHT;

    mDistances = new float[mScrWidth];
    mWallColours = new uint8_t[mScrWidth];
}

void Raycaster::Raycast() {
        Vec2f playerPos = mPlayerPtr->GetPos();
        Vec2f dirVec = mPlayerPtr->GetDirVec();
        Vec2f planeVec = mPlayerPtr->GetPlaneVec();

        for(int i=0; i < mScrWidth; i++) {
            float planeScalar = (2.0f * i / mScrWidth) - 1.0f;   // current point along viewing plane from -1 to 1

            Vec2f rayVec;
            rayVec.x = dirVec.x + (planeVec.x * planeScalar);   // ray vec will sweep along viewing plane
            rayVec.y = dirVec.y + (planeVec.y * planeScalar);   // as loop progresses (scaling plane vec)                           

            int tileX = floor(playerPos.x), tileY = floor(playerPos.y); // tile of the map ray currently is in

            // length of ray vector when x component is 1 (roughly), same thing for yStepRatio 
            float xStepRatio = (rayVec.x == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.x); // if will divide by zero, set infinity 
            float yStepRatio = (rayVec.y == 0) ? std::numeric_limits<float>::infinity() : std::abs(1 / rayVec.y); 

            // setting stepping values and starting pos for currDist

            int xStep, yStep;               // -1 or 1 depending on direction
            float currDistX, currDistY;     // distance from player to current point in loop

            if(rayVec.x < 0) { // x component in -ve direction
                xStep = -1;
                // this is total length along the ray vector to get to the next intersection with next x gridline
                // the difference between the player position and left gridline of tile is multiplied by step ratio
                currDistX = (playerPos.x - tileX) * xStepRatio;
            } else { // x component in +ve direction
                xStep = 1;
                // this time get x distance from tile to right, then multiply with ratio
                currDistX = ((tileX + 1.0f) - playerPos.x) * xStepRatio;
            }
            // same for y
            if(rayVec.y < 0) { // y component in -ve direction
                yStep = -1;
                // this is total length along the ray vector to get to the next intersection with next y gridline
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
                // currDistX/Y represent the length of the vector when the
                // next intersection of the gridline happens for an X or Y increment
                // so by incrementing whichever is smallest we get every tile that intersects with the line
                if(currDistX < currDistY) {
                    // increase x length to next intersection with gridlines
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
            if(wallSide == X_WALL) {
                // if the wall is on an x gridline CurrDistX needs to be used
                // as it holds the length between x gridlines
                // we stepped 1 too far in the final iteration so step back by 1 of xStepRatio
                mDistances[i] = currDistX - xStepRatio;
                // also store wall colours
                mWallColours[i] = mMapPtr[(tileY * mMapHeight) + tileX];
            } else {
                // same for y
                mDistances[i] = currDistY - yStepRatio;
                mWallColours[i] = mMapPtr[(tileY * mMapHeight) + tileX] + 10; // different wall colours for Y_WALLs
            }
        }
}

void Raycaster::Draw() {
    std::string scrBuffer;

    uint8_t prevColour = 0, currColour; // used to determine if to switch colour
    bool prevWasFloorCeil = false;

    int currLine = 0;
    for(int i=0; i < mScrHeight; i++) {
        // should get dist to centre of screen, might be off by 1 because im dumb
        float distFromCentre = (mScrHeight / 2.0f) - currLine; // non absolute
        //std::cout << distFromCentre << "\n";
        for(int j=0; j < mScrWidth; j++) {
            currColour = mWallColours[j];

            // inefficient, has to calculate more than necessary
            // precalculating would instead be faster
            float halfColumnHeight = mScrHeight / mDistances[j];

            //append character if column is there
            if(abs(distFromCentre) <= halfColumnHeight) { // wall
                // only add ansi colour when necessary
                if(currColour != prevColour || prevWasFloorCeil) {
                    int ANSIcolour = ANSIColourFromColour(currColour);

                }
                    
                scrBuffer.push_back(wallChar);
                prevWasFloorCeil = false;
            } else { // not wall
                if(distFromCentre > 0) { // ceiling
                    // only add ansi colour when necessary
                    if(!prevWasFloorCeil)
                        ;

                    scrBuffer.push_back(ceilChar);
                    prevWasFloorCeil = true;
                } else { // floor
                    // only add ansi colour when necessary
                    if(!prevWasFloorCeil)
                        ;

                    scrBuffer.push_back(floorChar);
                    prevWasFloorCeil = true;
                }
            }
            prevColour = currColour;
        }
        scrBuffer.push_back('\n');
        currLine++;
    }

    erase();
    wmove(mWindow, 0, 0);
    printw("%s", scrBuffer.c_str());
    wrefresh(mWindow);
}