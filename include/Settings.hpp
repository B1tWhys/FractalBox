#include <mutex>
#pragma once

struct Settings {
  std::mutex mutex;
  int FOV; // fov in degrees

  int width;
  int height;
  int recurseDepth;
};