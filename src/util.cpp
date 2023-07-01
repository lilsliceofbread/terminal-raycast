#include <sys/ioctl.h>
#include <unistd.h>
#include "util.hpp"

void getWinSize(int* w, int* h) {
    winsize terminal_size;

    // linux function to query kernel stuff
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);

    *w = terminal_size.ws_col;
    *h = terminal_size.ws_row;
}

bool getKeyPressed(KeySym key) {
    Display *display = XOpenDisplay(":0");
    char keysReturn[32]; // 32*8 = 256 bits for ascii codes

    XQueryKeymap(display, keysReturn);
    // gets actual ascii keycode - KeyCode is just unsigned char
    KeyCode keycode = XKeysymToKeycode(display, key); 

    // scuffed bitwise ops to get if bitflag of keyReturn is pressed using keycode
    // https://stackoverflow.com/questions/18281412/check-keypress-in-c-on-linux
    bool isPressed = (keysReturn[keycode >> 3] & (1 << (keycode & 7)));

    XCloseDisplay(display);
    return isPressed;
}