#ifdef __linux__
#include <ncurses.h>
#elif _WIN32
#include <curses.h>
#endif

#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>
#include <thread>
#include "util.hpp"
#include "player.hpp"
#include "raycaster.hpp"

/*
TODO:
make cross-platform using ncurses
*/

using namespace std::chrono_literals;

#define FRAME_LENGTH 16.66ms

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

    constexpr int mapWidth = 25;
    constexpr int mapHeight = 25;
    
    // 1 is white wall, 2 is red, 3 is yellow, 4 is magenta
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
        1,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,4,4,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,4,4,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    };

    //            x      y      a  mspeed tspeed
    Player player(12.0f, 12.0f, 0, 0.5f, 0.12f);

    // used in loop so needs initialisation
    float lastDeltaTime = 0; 

    Raycaster raycaster(map, mapWidth, mapHeight, &player);

    while(true) {
        int keyPressed = getch();
        if(keyPressed == 'q') { // check for quit key
            break;
        }
        ungetch(keyPressed); // put key back into input queue if not exit

        auto frameStart = std::chrono::system_clock::now();

        raycaster.Raycast();
        raycaster.Draw();

        // for debug purposes
        Vec2f playerPos = player.GetPos();
        char valueBuffer[100]; 
        // this is omega scuffed but i want to use string formatting
        sprintf(valueBuffer, "angle: %f x: %f y: %f \nframetime: %fms\n", player.GetAngle() * (180/M_PI), playerPos.x, playerPos.y, lastDeltaTime * 1000.0f);
        std::string valueStr(valueBuffer);
        raycaster.Print(valueStr);

        player.Update(map, mapWidth, mapHeight);

        auto frameEnd = std::chrono::system_clock::now();
        std::chrono::duration<float> frameTime = frameEnd - frameStart;
        lastDeltaTime = frameTime.count();                     // is last frame's time in seconds
        std::this_thread::sleep_for(FRAME_LENGTH - frameTime); // wait until end of frame (frame length - time already run)
    }

    return 0;
}