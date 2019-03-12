#include <vector>

class Camera {
    public:
        float fov;

        std::vector<float> loc;
        std::vector<float> vel;
        std::vector<float> accel; // Currently not used
        std::vector<float> lookDir;
};