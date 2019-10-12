#version 410 core
#include "../shaders/globals.frag"

struct MandelbrotDomain {
    double xOffset;
    double yOffset;
    double width;
    double height;
};


float f(float n, float H, float S, float L) {
    float k = n + H/30.0;
    k = k - floor(k/12.0)*12.0;
    float a;

    if (L < 0.5) {
        a = S*L;
    } else {
        a = S*(1-L);
    }

    return L - a * max(min(k-3, min(9-k, 1)), -1);
}

void main() {
    MandelbrotDomain domain;
    double aspectRatio = screenSize.y / screenSize.x;
    double scale = pow(.75, scrollUniform.y);
    domain.width = 2lf * scale;
    domain.height = 2lf * scale * aspectRatio;
    domain.xOffset = loc2d.x * 0.15lf - domain.width / 2lf;
    domain.yOffset = loc2d.y * 0.15 - domain.height / 2;

    double x0 = domain.width * gl_FragCoord.x / screenSize.x + domain.xOffset;
    double y0 = domain.height * gl_FragCoord.y / screenSize.y + domain.yOffset;
    double x = 0.0;
    double y = 0.0;
    double xTmp = 0;
    bool inSet = true;
    double iteration = 0;
    double max_iteration = 100;
    float nu;
    float log_zn;

    while (iteration < max_iteration && x*x + y*y <= 4) {
        xTmp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xTmp;
        iteration += 1;
    }

    if (iteration < max_iteration) {
        nu = log(log(float(x * x + y * y)) / 2.0f / log(2.0)) / log(2.0);
        iteration = iteration + 1 - nu;
    } else if (iteration == max_iteration) {
        iteration = 0;
    }

    float ratio;
    ratio = pow(float(iteration/max_iteration), 1.0);

    float H = 360.0 * ratio;
    float S = 1 - ratio;
    float L = ratio;

    float r = f(0.0, H, S, L);
    float g = f(8.0, H, S, L);
    float b = f(4.0, H, S, L);

    FragColor = vec4(r, g, b, 1.0);
}
