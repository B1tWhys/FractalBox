#include "App.hpp"

App::App() {}

App &App::getInstance() {
    static App instance;
    return instance;
}

void App::run() {
    this->engine.run();
}