#version 330
uniform float iTime;
in vec2 fCoord;
out vec4 fColor; 
void main () {
    fColor = vec4(fCoord,1,1);
}
