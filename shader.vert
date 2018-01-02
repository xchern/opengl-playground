#version 330

uniform mat4 proj;
uniform vec3 eyePos;

in vec3 vPos;
in vec3 vNorm;
out vec3 fNorm;
out vec3 fEye;

void main () {
    gl_Position = proj * vec4(vPos, 1.);
    fNorm = vNorm;
    fEye = eyePos - vPos;
}