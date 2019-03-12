#version 410 core

struct MandelbrotDomain {
    double xOffset;
    double yOffset;
    double width;
    double height;
};

uniform dvec2 screenSize;
uniform MandelbrotDomain domain;

in vec4 gl_FragCoord;
out vec4 FragColor;

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
    // double x0 = domain.width * gl_FragCoord.x / screenSize.x + domain.xOffset;
    // double y0 = domain.height * gl_FragCoord.y / screenSize.y + domain.yOffset;
    double x0 = 3.5 * gl_FragCoord.x / 1920 - 2.5;
    double y0 = 2.0 * gl_FragCoord.y / 1080 - 1;
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