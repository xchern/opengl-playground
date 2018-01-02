#version 330

uniform vec3 lightDir;

in vec3 fNorm;
in vec3 fEye;

out vec3 fColor; 

const float ks = 1;
const float kd = .8;
const float ka = .2;
const float alpha = 128;

const vec3 ia = vec3(1,0.8,0);
const vec3 id = vec3(1,0.8,0);
const vec3 is = vec3(1,1,1);

void main () {
    vec3 v = normalize(fEye);
    vec3 n = normalize(fNorm);
    vec3 l = lightDir;
    vec3 r = reflect(-l, n);

    float diff = kd * max(0,dot(l, n));
    float spec = ks * pow(max(0,dot(v, r)),alpha);

    fColor = ka * ia + diff * id + spec * is;
}