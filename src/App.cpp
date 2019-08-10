#include "App.hpp"

App::App() : engine(&settings), cli(&settings) {
    
}
App::~App() {
    if (this->engineThread.joinable()) {
        this->engineThread.join();
    }
}

// App::App(): engine(Engine(&settings)), cli(CLI(&settings)) { }

App &App::getInstance() {
    static App instance;
    return instance;
}

void App::run() {
#ifndef __APPLE__
    this->engineThread = std::thread(&Engine::run, std::ref(this->engine));
#else
    this->engine.run();
#endif

    // this->cliThread = std::thread(&CLI::run, this->cli);
}
