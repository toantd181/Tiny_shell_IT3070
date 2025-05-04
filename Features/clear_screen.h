#ifndef CLEAR_SCREEN_H
#define CLEAR_SCREEN_H

#include<iostream>

void clearScreen(){
    // \033[2J: clear the screen
    // \033[H: move the cursor to the top-left corner]
    std::cout << "\033[2J\033[H";
    std::cout.flush(); // Excuted immidiately
};

#endif
