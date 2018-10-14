#version 330

float diffuse(vec3 l, vec3 n, vec3 v, float kd) {
    float c = dot(l, n);
    if (c > 0.) return kd * c;
    else return 0.;
}

float spectrum(vec3 l, vec3 n, vec3 v, float ks, float alpha) {
    float c = dot(l, n);
    if (c <= 0.) return 0.;
    vec3 r = reflect(-l, n);
    return ks * pow(max(0,dot(v, r)),alpha);
}

struct phong_config {
    float ks, kd, alpha;
};

const float ki = .8;
const float ks = .2;
const float kd = .8;
const float alpha = 8;

const vec3 ia = vec3(1,1,1) * 0.2;
const vec3 id = vec3(1,1,1);
const vec3 is = vec3(1,1,1);

vec3 brdf(vec3 l, vec3 n, vec3 v) {
    float diff = diffuse(l, n, v, kd);
    float spec = spectrum(l, n, v, ks, alpha);
    return kd * ia + diff * id + spec * is;
}
