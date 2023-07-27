# WIP

Basic raycaster in the terminal. 
Move around with WASD, turn with left and right arrow keys, q to quit. 
Issues:

- Cannot handle multiple inputs due to ncurses limitations

- Random "screen tearing"/lines not drawing to screen

On linux you need to install the ncurses package 
`sudo apt install libncurses-dev`
and then run `make`

On Windows you need to install MinGW for compilation. To compile, run `make -f Winmakefile` in the project directory.
