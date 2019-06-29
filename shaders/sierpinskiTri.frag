#version 410 core

#define M_PI 3.1415926535897932384626433832795

in vec4 gl_FragCoord;

uniform vec2 screenSize;
uniform float FOV;
uniform mat4 camToWorldMat;
uniform mat4 worldToCamMat;
// uniform int maxSteps;
// uniform float minDist;

out vec4 FragColor;

const float minDist = 0.005;
const int maxSteps = 500;

// TODO: make these uniforms
const int iterations = 8;
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
float brightness = 8;
const vec3 ambientColor = vec3(0.1);
vec3 diffuseColor = vec3(1.);
const vec3 specColor = vec3(2.0);
const float shininess = 4.;
const float screenGamma = 2.2;
const float AO = 0.1;

const float epsilon = 0.00001;


float DE_triangle(vec3 p) {
    /* these are NOT the vertices of the tetrahedron!! They are actually the normal 
    vectors for each face, although this also happens to be a larger tetrahedron.
    These coordinates form a tetrahedron whose points are all on the unit sphere centered
    on the origin, but this one points 'downward' while the one being rendered points upward
    and is much smaller (2/3, actually). Another way to think of this is that the tetrahedron that's actually
    rendered has vertices which exactly touch the middle of each of the faces of this one.
    
    extra notes:
    the edge length of the bounding tet. is sqrt(8/3) and the height (from vert to opp. face)
    is 4/3
    */
    
    // Edit: actually these are not relevant at all. all the tetrahedron stuff is currently done via space folding
    // symmetries.
    vec3 n1 = vec3(-sqrt(8./.9), 0, 1./.3);
    vec3 n2 = vec3(sqrt(2./.9), -sqrt(2./3.), 1./3.);
    vec3 n3 = vec3(sqrt(2./.9), sqrt(2./3.), 1./3.);
    vec3 n4 = vec3(0, 0, -1);
    n1 = -n1;
    n2 = -n2;
    n3 = -n2;
    n4 = -n2;

    vec3 c = n3;
    float dist, d;
    // float scale = 2;
    float scale = 2;
    
    int n;
    for (n = 0; n < iterations; n++) {
        // if (p.x < 0.) p.x = -p.x;
        // if (p.y < 0.) p.y = -p.y;
        // if (p.z < 0.) p.z = -p.z;

        if(p.x+p.y<0.0) p.xy = -p.yx;
        if(p.x+p.z<0.0) p.xz = -p.zx;
        if(p.y+p.z<0.0)p.zy = -p.yz;

        p = p*scale - 1;
    }
    
    // return dot(p-c, c);
    // return length(p) - .2;
    return (length(p) * pow(scale, float(-n)));
}

float DE_ground(vec3 p) {
    // return abs(p.x + p.y);
    return abs(p.y) + 2;
}

float DE(vec3 p) {
    float dist;
    dist = min(DE_ground(p), DE_triangle(p));
    // dist = DE_triangle(p);
    return dist;
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
    vec4 intersect = trace(startPt, endPt-startPt);
    return isinf(intersect.x);
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

    vec3 rayDir = rayDirection()*.25;
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

    vec3 color;
    if (true) {
        float base = 8;
        color = vec3(pow(base, stepCount/maxSteps)/base);
        FragColor = vec4(color, 1.0);
    } else {
        vec3 lightPos = vec3(0, 15, 0);
        color += blinnPhong(rayIntersect, n, -rayDir, lightPos);
        // color -= vec3(stepCount/maxSteps)*AO;
        FragColor = vec4(color, 1.0);
    }
}
