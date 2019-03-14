#include "Engine.hpp"
#include "Settings.hpp"
#include <iostream>

#pragma once
/*
This is a singleton object which represents the Application. Currently it
only initializes and maintains a pointer to an Engine object, however in
the future there will be an additional 'settings' window either on another
thread or in another process, and this will manage the communiation between
that and the Engine instance.
*/

class App {
    public:
        static App& getInstance();

        App(App const&)               = delete;
        void operator=(App const&)    = delete;

        void run();

        Engine engine;
        Settings settings;
    private:
        App();
        void initSettings();
};