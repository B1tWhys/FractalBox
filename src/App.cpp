#include "App.hpp"

App::App() {
  initSettings();
}

void App::initSettings() {
    this->settings.FOV = 90;
    this->settings.height = 1920;
    this->settings.width = 1080;
    this->settings.recurseDepth = 100;
}

App &App::getInstance() {
    static App instance;
    return instance;
}

void App::run() {
    this->engine.run();
}
