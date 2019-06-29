#version 410 core

in vec4 gl_FragCoord;

uniform vec2 screenSize;
uniform float FOV;
uniform mat4 camToWorldMat;
uniform mat4 worldToCamMat;
uniform int maxSteps;
uniform float minDist;

out vec4 FragColor;

// TODO: make these uniforms
const int iterations = 5;
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
float brightness = 20;
const vec3 ambientColor = vec3(0.1);
vec3 diffuseColor = vec3(0.8);
const vec3 specColor = vec3(2.0);
const float shininess = 4.;
const float screenGamma = 2.2;
const float AO = 0.1;

const float epsilon = 0.00001;

float DE_triangle(vec3 p) {
    vec3 a1 = vec3(1, 1, 1);
    vec3 a2 = vec3(-1, -1, 1);
    vec3 a3 = vec3(1, -1, -1);
    vec3 a4 = vec3(-1, 1, -1);
    vec3 c;
    float dist, d;
    float scale = 0.5;

    int n;
    for (n = 0; n < iterations; n++) {
        c = a1;
        dist = length(p - a1);
        
        d = length(p-a2);
        if (d < dist) {c = a2; dist = d;}
        
        d = length(p-a3);
        if (d < dist) {c = a3; dist = d;}

        d = length(p-a4);
        if (d < dist) {c = a4; dist = d;}
        
        p = p*scale-c*(scale-1.0);
    }
    
    // return length(p) - 3;
    return length(p) * pow(scale, float(-n));
}

float DE_ground(vec3 p) {
    return p.y;
}
float DE(vec3 p) {
    float minDist = DE_ground(p);
    minDist = min(minDist, DE_triangle(p));
    return minDist;
}

// from = source of the ray (either a camera or the last ray bounce)
// dir = unit vector in the direction of the ray
// returns a vec4 where x,y,z are the coords of the intersect of the ray with a surface
//      and w is the number of steps it took to get there
vec4 trace(vec3 from, vec3 dir) {
    float totalDistance = 0.0;
    int steps = 0;
    
    for (steps = 0; steps < maxSteps; steps++) {
        // vec3 p = from + totalDistance * dir * .8;
        vec3 p = from + totalDistance * dir;
        float distance = DE(p);
        totalDistance += distance;
        if (distance < minDist) {
            return vec4(p, steps);
        } /*else if (false && totalDistance > 30) {
            return vec4(1./0);
        }*/
    }
    
    return vec4(1./0);
}

vec3 rayDirection() {
    float normedX = (gl_FragCoord.x - screenSize.x/2) / screenSize.x;
    float normedY = (gl_FragCoord.y - screenSize.y/2) / screenSize.x;
    
    vec4 ray = vec4(normedX, normedY, -1, 0);
    ray = camToWorldMat * ray;
    return normalize(vec3(ray));
}

bool isObstructed(vec3 startPt, vec3 endPt) {
    // return false;
    vec3 dir = normalize(endPt - startPt);
    vec3 current = startPt;
    float dist = length(startPt - endPt);

    for (int i = 0; i < maxSteps; i++) {
        float de = DE(current);
        if (de < epsilon) return true;
        current += de * dir;
        dist = length(endPt-current);
    }
    
    return false;
}

vec3 blinnPhong(vec3 surfacePt, vec3 surfaceNorm, vec3 rayDir, vec3 lightPos) {
    // lightPos.xz = mod((lightPos.xz), 20) + vec2(8);
    vec3 lightDir = lightPos - surfacePt;
    float distance = pow(length(lightDir), 2);
    lightDir = normalize(lightDir);
    
    float lambertian = dot(lightDir, surfaceNorm);
    float specular = 0.0;
    
    // if (!isObstructed(surfacePt, lightPos)) {
    if (lambertian > 0.) {
        vec3 halfDir = normalize(rayDir + lightDir);
        float specAngle = max(dot(halfDir, surfaceNorm), 0.0);
        specular = pow(specAngle, shininess);
    }  

    vec3 colorLinear = ambientColor +
                       diffuseColor * lambertian * lightColor * brightness / distance +
                       specColor * specular * lightColor * brightness / distance;
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));
    return colorLinear;
}

void main() {
    vec3 xDir = vec3(1, 0, 0);
    vec3 yDir = vec3(0, 1, 0);
    vec3 zDir = vec3(0, 0, 1);

    vec3 rayDir = rayDirection();
    vec3 camPos = vec3(camToWorldMat * vec4(0, 0, 0, 1));
    vec4 rayIntersectStepCount = trace(camPos, rayDir);
    vec3 rayIntersect = vec3(rayIntersectStepCount);
    float stepCount = rayIntersectStepCount.w;

    if (isinf(rayIntersect.x)) {
        FragColor = vec4(196, 228, 255, 255)/255.;
        return;
    } else if (rayIntersect.y <= epsilon) {
        diffuseColor *= .5;
    }

    // surface normal
    float delta = 0.0001;
    vec3 n = normalize(vec3(DE(rayIntersect + xDir * delta) - DE(rayIntersect - xDir * delta),
                            DE(rayIntersect + yDir * delta) - DE(rayIntersect - yDir * delta),
                            DE(rayIntersect + zDir * delta) - DE(rayIntersect - zDir * delta)));

    vec3 color = vec3(0);
    vec3 lightPos = rayIntersect + vec3(2, 8, 4);
    color += blinnPhong(rayIntersect, n, -rayDir, lightPos);
    color -= vec3(stepCount/maxSteps)*AO;
    FragColor = vec4(color, 1.0);
}
