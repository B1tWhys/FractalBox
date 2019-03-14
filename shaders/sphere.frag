#version 410 core

in vec4 gl_FragCoord;

uniform vec2 screenSize;
uniform float FOV;
uniform mat4 camToWorldMat;
uniform mat4 worldToCamMat;
uniform int maxSteps;
uniform float minDist;

out vec4 FragColor;

float DE(vec3 p) {
    float R = 0.5;
    return (length(p) - R) * .06;
}

// from = source of the ray (either a camera or the last ray bounce)
// dir = unit vector in the direction of the ray
float trace(vec3 from, vec3 dir) {
    float totalDistance = 0.0;
    int steps = 0;

    for (steps = 0; steps < maxSteps; steps++) {
        vec3 p = from + totalDistance * dir;
        float distance = DE(p);
        totalDistance += distance;
        if (distance < minDist) {
            break;
        }
    }

    return 1.0-float(steps)/float(maxSteps);
}

vec3 rayDirection() {
    float normedX = gl_FragCoord.x/screenSize.x - 0.5;
    float normedY = (gl_FragCoord.y - screenSize.y/2) / screenSize.x;// - 0.5;

    vec4 ray = vec4(normedX, 1, normedY, 0);
    ray = camToWorldMat * ray;
    return normalize(vec3(ray));
}

void main() {
    float b = trace(vec3(camToWorldMat * vec4(0, 0, 0, 1)), rayDirection());
    // float b;
    // vec4 camPos = camToWorldMat * vec4(0, 1, 0, 1);
    // float val = camPos.y;
    // float cmp = -2.0;
    // if (val < cmp) {
    //     b = 0;
    // } else if (val == cmp) {
    //     b = 0.5;
    // } else if (val > cmp) {
    //     b = 1;
    // }
    FragColor = vec4(b, b, b, 1.0);
}