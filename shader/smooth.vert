#version 330

uniform mat4 proj;
uniform mat4 trans;
uniform vec3 eyePos;

in vec3 vPos;
in vec3 vNorm;
out vec3 fNorm;
out vec3 fEye;

void main () {
    vec4 p = trans * vec4(vPos, 1);
    gl_Position = proj * p;
    fNorm = (trans * vec4(vNorm, 0)).xyz;
    fEye = eyePos - p.xyz;
}
