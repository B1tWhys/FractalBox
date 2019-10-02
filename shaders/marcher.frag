
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
        } else if (totalDistance > maxDist) {
            return vec4(1./0);
        }
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
    }
    // surface normal
    float delta = 0.0001;
    vec3 n = normalize(vec3(DE(rayIntersect + xDir * delta) - DE(rayIntersect - xDir * delta),
                            DE(rayIntersect + yDir * delta) - DE(rayIntersect - yDir * delta),
                            DE(rayIntersect + zDir * delta) - DE(rayIntersect - zDir * delta)));

    vec3 color;
    if (false) {
        float base = 8;
        color = vec3(pow(base, stepCount/maxSteps)/base);
        FragColor = vec4(color, 1.0);
        if (rayIntersect.y < -1) {
            FragColor = vec4(.8, .8, .8, 1.);
        }
        if (rayIntersect.x > 0) {
            FragColor += vec4(.2, 0, 0, 0);
        } 
        
        if (rayIntersect.z > 0) {
            FragColor += vec4(0, .2, 0, 0);
        }
    } else {
        vec3 lightPos = vec3(0, 15, 0);
        color += blinnPhong(rayIntersect, n, -rayDir, lightPos);
        // color -= vec3(stepCount/maxSteps)*AO;
        FragColor = vec4(color, 1.0);
    }
}
