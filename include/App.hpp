#include "Engine.hpp"
/*
This is a singleton object which represents the Application. Currently it
only initializes and maintains a pointer to an Engine object, however in
the future there will be an additional 'settings' window either on another
thread or in another process, and this will set up the IPC between the 
settings and the engine.
*/

class App {
    public:
        static App& getInstance();

        App(App const&)               = delete;
        void operator=(App const&)    = delete;

        void run();
    private:
        App();
        Engine engine;
};