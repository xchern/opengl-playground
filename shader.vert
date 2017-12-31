#version 330

uniform vec2 translation;

in vec2 vPos;

void main () {
    gl_Position = vec4(vPos + translation, 0., 1.);
}