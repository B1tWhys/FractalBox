#include <mutex>
#pragma once

struct Settings {
  std::mutex lock;

  // screen resolution
  int width = 1920; 
  int height = 1080;

  int maxSteps = 300; // maximum number of ray steps
  float minDist = .001; // smallest possible ray step

  float mouse_sensitivity = 1.0f/50; // lower number is slower camera movement
};