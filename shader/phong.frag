#version 330

const float ks = .2;
const float kd = .8;
const float ka = .1;
const float alpha = 8;

const vec3 ia = vec3(1,0.5,0);
const vec3 id = vec3(1,0.5,0);
const vec3 is = vec3(1,1,1);

vec3 brdf(vec3 v, vec3 n, vec3 l) {
    vec3 r = reflect(-l, n);

    float diff = kd * max(0,dot(l, n));
    float spec = ks * pow(max(0,dot(v, r)),alpha);

    return ka * ia + diff * id + spec * is;
}