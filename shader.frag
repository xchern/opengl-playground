#version 330

out vec3 fColor; 
in vec3 fNorm;

const vec3 l = normalize(vec3(1,.1,0));
const float ks = 1;
const float kd = .7;
const float ka = .2;
const float alpha = 4;

const vec3 ia = vec3(1,0.8,0);
const vec3 id = vec3(1,0.8,0);
const vec3 is = vec3(1,1,1);

void main () {
    vec3 v = vec3(0,0,1);
    vec3 n = normalize(fNorm);
    vec3 r = reflect(-l, n);

    float diff = kd * max(0,dot(l, n));
    float spec = ks * pow(max(0,dot(r, v)),alpha);

    fColor = ka * ia + diff * id + spec * is;
}