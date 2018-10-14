#version 330
layout (location = 0) in vec2 vPos;

out vec2 fCoord;
void main () {
    fCoord = (vPos + 1.) * 0.5;
    gl_Position = vec4(vPos, 0, 1);
}
