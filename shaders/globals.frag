in vec4 gl_FragCoord;

uniform vec2 screenSize;
uniform float FOV;
uniform mat4 camToWorldMat;
uniform mat4 worldToCamMat;
// uniform int maxSteps;
// uniform float minDist;

out vec4 FragColor;

const float minDist = .0001;
const float maxDist = 200.;
const int maxSteps = 800;

// TODO: make these uniforms
const int iterations = 8;
const vec3 lightColor = vec3(2.0, 1.0, 1.0);
float brightness = 8.;
const vec3 ambientColor = vec3(0.1);
vec3 diffuseColor = vec3(1.);
const vec3 specColor = vec3(2.0);
const float shininess = 4.;
const float screenGamma = 2.2;
const float AO = 0.1;

const float epsilon = 0.00001;

