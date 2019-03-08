#include "App.hpp"

App::App() {}

App &App::getInstance() {
    static App instance;
    return instance;
}