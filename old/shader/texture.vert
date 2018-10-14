#version 330

uniform mat4 proj;
uniform vec3 eyePos;

in vec3 vPos;
in vec3 vNorm;
in vec2 vCoord;
in mat4 trans;

out vec3 fNorm;
out vec3 fEye;
out vec2 fCoord;

void main () {
    vec4 p = trans * vec4(vPos, 1);
    gl_Position = proj * p;
    fNorm = (trans * vec4(vNorm, 0)).xyz;
    fEye = eyePos - p.xyz;
    fCoord = vCoord;
}
