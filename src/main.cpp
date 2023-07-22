#ifdef __linux__
#include <ncurses.h>
#elif _WIN32
#include <curses.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
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
    Player player(12.0f, 12.0f, 0, 400.0f, 100.0f);

    Raycaster raycaster(map, mapWidth, mapHeight, &player);


    // ask if to use unicode block character
    /*std::cout << "Use Unicode characters? [y for yes]" << std::endl;
    std::string answer;
    std::getline(std::cin, answer);

    std::string yes = "Yy";

    if(yes.find(answer) != std::string::npos) { // if answer is matched in yes string
        raycaster.SetUnicode();
    } // all other options are the default*/



    auto gameStartTime = std::chrono::system_clock::now();
    float lastDeltaTime = 0.003; // reasonable starting value
    int totalFrames = 0;
    raycaster.InitWindow();

    // game loop
    while(true) {
        int keyPressed = getch();
        if(keyPressed == 'q') {
            break;
        }
        ungetch(keyPressed); // put key back into input queue if not exit

        auto frameStart = std::chrono::system_clock::now();

        // FIXXXXXXXXXXXXXX
        Vec2f playerPos = player.GetPos();
        std::string printString;
        printString = "angle: " + std::to_string(player.GetAngle() * (180/M_PI)) + " x: " + std::to_string(playerPos.x) + " y: " + std::to_string(playerPos.y) + "\n";

        raycaster.Raycast();
        raycaster.Draw();

        raycaster.Print(printString);

        player.Update(map, mapWidth, mapHeight, lastDeltaTime);

        totalFrames++;

        auto frameEnd = std::chrono::system_clock::now();
        std::chrono::duration<float> frameTime = frameEnd - frameStart;

        // FIX PART 2
        std::stringstream testString;
        testString << "total frames: " << totalFrames << " frame time: " << frameTime.count() * 1000 << "ms\n";
        raycaster.Print(testString.str());

        lastDeltaTime = frameTime.count();

        // wait until end of frame (frame length - time already run)
        std::this_thread::sleep_for(FRAME_LENGTH - frameTime); 
    }

    std::chrono::duration<float> totalTime = (std::chrono::system_clock::now()) - gameStartTime;
    std::cout << "total time running: " << totalTime.count() << "s" << std::endl;
    
    return 0;
}