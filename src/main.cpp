#include <iostream>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "X11/keysym.h"
//TODO:
//GITHUB

void getWinSize(int* w, int* h) {
    winsize terminal_size;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);

    *w = terminal_size.ws_col;
    *h = terminal_size.ws_row;
}

bool getKeyPressed(KeySym key) {
    Display *display = XOpenDisplay(":0");
    char keysReturn[32];

    XQueryKeymap(display, keysReturn);
    KeyCode keycode = XKeysymToKeycode(display, key);
    bool isPressed = !!(keysReturn[keycode >> 3] & (1 << (keycode & 7)));
    XCloseDisplay(display);
    return isPressed;
}

int main() {
    using namespace std::this_thread;     
    using namespace std::chrono_literals; 
    //using std::chrono::system_clock;

    bool run = true;
    int width, height;

    getWinSize(&width, &height);

    std::cout << "\x1b[2J\x1b[H"; 
    while(run) {
        //kinda works, have to hold q for ~200ms
        //find way to get current keys pressed?
        //may be over optimising
        if(getKeyPressed(XK_q)) {
            run = false;
            break;
        }
        std::cout << "hmm" << std::endl;
        sleep_for(200ms);
    }
    
    return 0;
}