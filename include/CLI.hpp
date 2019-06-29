#include "Settings.hpp"
#include <ncurses.h>
#include <functional>
#include <mutex>
#pragma once

/*
This is where the code for the Command Line graphical user Interface lives. The App initializes an instance
of this object, sets the CLI's settings pointer, and then calls run() on this object in another thread, which
starts the loop which presents all the command line stuff to the user. 

When a change is made here, a new Settings struct is created and then passed to the App
singleton via a callback. The App object then does the appropriate stuff to update the
Engine (usually it updates Engine's Settings struct, then calls the funciton to rebuild
the render pipeline).
*/

void dbgLog(const char *format...);

class CLI {
    public:
        CLI(Settings *);
        ~CLI();
        void run();
    private:
        Settings *settings;
};