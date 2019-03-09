#version 460 core

struct MandelbrotDomain {
    double xOffset;
    double yOffset;
    double width;
    double height;
};

uniform dvec2 screenSize;
uniform MandelbrotDomain domain;

out vec4 FragColor;

float f(float n, float H, float S, float L) {
    float k = n + H/30f;
    k = k - floor(k/12f)*12f;
    float a;

    if (L < 0.5) {
        a = S*L;
    } else {
        a = S*(1-L);
    }

    return L - a * max(min(k-3, min(9-k, 1)), -1);
}

void main() {
    // float x0 = 3.5 * gl_FragCoord.x / screenSize.x - 2.5;
    // float y0 = domain.height * gl_FragCoord.y / screenSize.y + domain.yOffset;
    // float x0 = domain.width * gl_FragCoord.x / screenSize.x + domain.xOffset;
    // float y0 = domain.height * gl_FragCoord.y / screenSize.y + domain.yOffset;
    // float x = 0.0;
    // float y = 0.0;
    // float xTmp = 0;
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
        nu = log(log(float(x * x + y * y)) / 2.0f / log(2f)) / log(2f);
        iteration = iteration + 1 - nu;
    }
    float ratio;
    ratio = pow(float(iteration/max_iteration), 1.0);

    float H = 360f * ratio;
    float S = 1 - ratio;
    float L = ratio;


    // float r = sqrt(ratio);
    // float g = pow(ratio, 1.0f/3.0f);
    // float b = pow(ratio, 1.0f/4.0f);;
    float r = f(0f, H, S, L);
    float g = f(8f, H, S, L);
    float b = f(4f, H, S, L);

    FragColor = vec4(r, g, b, 1.0f);

    // float c;
    // c = domain.xOffset;
    // FragColor = vec4(c, c, c, 1.0f);
}