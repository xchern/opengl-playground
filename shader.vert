#version 330

uniform vec2 trans;

in vec2 vPos;

void main () {
    gl_Position = vec4(vPos + trans, 0., 1.);
}