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
    this->engineThread = std::thread(&Engine::run, std::ref(this->engine));
    // this->engine.run();
    // this->cliThread = std::thread(&CLI::run, this->cli);
}
