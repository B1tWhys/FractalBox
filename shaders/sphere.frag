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
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
float brightness = 40;
const vec3 ambientColor = vec3(0.1);
vec3 diffuseColor = vec3(0.8);
const vec3 specColor = vec3(2.0);
const float shininess = 4.;
const float screenGamma = 2.2;

const float epsilon = 0.00001;

float DE_sphere_instanced(vec3 p) {
    p.xz = mod((p.xz), 1.0) - vec2(0.5);
    p.y -= .3;
    return length(p)-0.3;
}

float DE_sphere(vec3 p) {
    vec3 origin =  vec3(0, .3, 0);
    vec3 gridDims = vec3(1, 1, 1); // the size of the grid of spheres
    vec3 sphereSpacing = vec3(2);

    float R = 0.3;
    float leastDist = 1./0.;

    p.xz = mod((p.xz), 2.0) - vec2(.5);
    vec3 center;
    float dist;
    for (int x = 0; x < gridDims.x; x += 1) {
        for (float y = 0; y < gridDims.y; y += 1) {
            for (float z = 0; z < gridDims.z; z += 1) {
                center = origin + sphereSpacing * vec3(x, y, z);
                // dist = max(length(p-center) - R, 0);
                dist = length(p-center) - R;
                if (dist < leastDist) leastDist = dist;
            }
        }
    }
    return leastDist;
}

float DE_ground(vec3 p) {
    return p.y;
}

float DE(vec3 p) {
    float minDist = DE_ground(p);
    
    // minDist = min(minDist, DE_sphere(p));
    minDist = min(minDist, DE_sphere_instanced(p));
    return minDist;
}

// from = source of the ray (either a camera or the last ray bounce)
// dir = unit vector in the direction of the ray
// returns a vec4 where x,y,z are the coords of the intersect of the ray with a surface
//      and w is the number of steps it took to get there
vec4 trace(vec3 from, vec3 dir) {
    float totalDistance = 0.0;
    int steps = 0;
    
    for (steps = 0; steps < maxSteps * 2; steps++) {
        // vec3 p = from + totalDistance * dir * .8;
        vec3 p = from + totalDistance * dir;
        float distance = DE(p);
        totalDistance += distance;
        if (distance < minDist) {
            return vec4(p, 1.);
        } else if (false && totalDistance > 30) {
            return vec4(1./0);
        }
    }
    
    return vec4(1./0);
}

vec3 rayDirection() {
    float normedX = (gl_FragCoord.x - screenSize.x/2) / screenSize.x;
    float normedY = (gl_FragCoord.y - screenSize.y/2) / screenSize.x;
    
    vec4 ray = vec4(normedX, normedY, 1, 0);
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
    vec3 lightDir = lightPos;
    lightDir.xz = mod((lightDir.xz), 50) - vec2(25);
    float distance = pow(length(lightDir), 2);
    lightDir = normalize(lightDir);
    
    float lambertian = dot(lightDir, surfaceNorm);
    float specular = 0.0;
    
    if (lambertian > 0.) { // && (false || !isObstructed(surfacePt + surfaceNorm * .01, lightPos))) {
        vec3 halfDir = normalize(rayDir + lightDir);
        float specAngle = max(dot(halfDir, surfaceNorm), 0.0);
        specular = pow(specAngle, shininess);
    } else {
        brightness = 0.;
    }
    
    vec3 colorLinear = diffuseColor * lambertian * lightColor * brightness / distance +
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
    float stepCount =  rayIntersectStepCount.w;

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

    float lightSpacing = 40;
    const float lightY = 5;
    vec3 color = ambientColor;
    // for (int x = -1; x <= 1; x++) {
    //     for (int y = -1; y <= 1; y++) {
    //     }
    // }
    // vec2 lightOffset = mod((rayIntersect.xz), lightSpacing) - lightSpacing/2;
    // vec3 lightPos = vec3(rayIntersect.x + lightOffset.x, lightY, rayIntersect.z + lightOffset.y);
    vec3 lightPos = vec3(8, 3, 0);
    color += blinnPhong(rayIntersect, n, -rayDir, lightPos);
    lightPos = vec3(-3, -8, 4);
    color += blinnPhong(rayIntersect, n, -rayDir, lightPos);
    // blinnPhong(rayIntersect, n, -rayDir, lightPos);
    FragColor = vec4(color, 1.0);
}