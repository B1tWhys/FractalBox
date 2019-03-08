#include "App.hpp"
#include <iostream>

App::App() {
    this->engine = Engine();
}

App &App::getInstance() {
    static App instance;
    return instance;
}

void App::run() {
    this->engine.run();
}