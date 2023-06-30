#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <thread>
#include "util.hpp"

using namespace std::this_thread;     
using namespace std::chrono_literals;
using std::chrono::duration;
using std::chrono::system_clock;

/*
TODO:
make cross-platform using ncurses
*/

int main() {
    int winWidth, winHeight;
    getWinSize(&winWidth, &winHeight);

    const int gameWidth = (winWidth < 30) ? winWidth : 30;
    const int gameHeight = (winHeight < 20) ? winHeight : 20;
   
    std::string screen[gameHeight];

    int totalFrames = 0;
    while(true) {
        if(getKeyPressed(XK_q)) {
            break;
        }

        auto startTime = system_clock::now();

        std::system("clear"); // bad fix later
        std::cout << "\x1b[2J\x1b[H"; 

        // draw columns to string
        // i think doing this intermediate step will be faster
        for(std::string& line : screen) {
            line = ""; // remove previous line
            for(int i=0; i<gameWidth; i++) {
                line.append("y");
            }
        }

        for (const std::string& line : screen) {
            // i think std::endl is slow?
            std::cout << line << "\n";
        }

        totalFrames++;
        std::cout << "total frames: " << totalFrames << "\n";

        // wait until end of frame (frame length - time already run)
        // might cause problems if negative values of time
        sleep_for(50ms - (system_clock::now() - startTime)); 
    }
    
    return 0;
}