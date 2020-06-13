#version 330

const float refr = 1.6;

const float cosAng = cos(.3);
const float ka = .1;

const vec3 ia = vec3(1,0.5,0);
const vec3 id = vec3(1,0.5,0);
const vec3 is = vec3(1,1,1);

float fresnel(float cosi, float n) {
    float cost2 = 1. - (1. - cosi * cosi) * (1./(n*n));
    if (cost2 < 0.) return 1.;
    float cost = sqrt(cost2);
    float Rs = pow((n * cosi - cost)/(n * cosi + cost),2.);
    float Rp = pow((n * cost - cosi)/(n * cost + cosi),2.);
	return .5*(Rs + Rp);
} 

vec3 marble(vec3 v, vec3 n, vec3 l,
           vec3 ia, vec3 id, vec3 is,
           float ka, float kd, float ks,
           float cosAng) {
    vec3 r = reflect(-v, n);

    float diff = kd * max(0,dot(l, n));
    float spec = ks * step(cosAng, dot(l, r));

    return ka * ia + diff * id + spec * is;
}

vec3 brdf(vec3 v, vec3 n, vec3 l) {
    float cosi = dot(v,n);
    float ks = fresnel(cosi, refr);
    float kd = 1.-ks;
    return marble(v,n,l,ia,id,is,ka,kd,ks,cosAng);
}