#version 330

uniform mat4 proj;

in vec3 vPos;
in vec3 vNorm;
out vec3 fNorm;

void main () {
    gl_Position = proj * vec4(vPos, 1.);
    fNorm = (proj * vec4(vNorm,1.)).xyz;
}