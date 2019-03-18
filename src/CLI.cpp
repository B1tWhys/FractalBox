#include "CLI.hpp"
#include <assert.h>

CLI::CLI(Settings *settings) {
    this->settings = settings;
}

CLI::~CLI() {
    endwin();
}

void CLI::run() {
    assert(this->settings != nullptr);
    
}