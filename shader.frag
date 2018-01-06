#version 330

uniform vec3 lightDir;

in vec3 fNorm;
in vec3 fEye;

out vec3 fColor; 

vec3 phong(vec3 v, vec3 n, vec3 l,
           vec3 ia, vec3 id, vec3 is,
           float ka, float kd, float ks,
           float alpha) {
    vec3 r = reflect(-l, n);

    float diff = kd * max(0,dot(l, n));
    float spec = ks * pow(max(0,dot(v, r)),alpha);

    return ka * ia + diff * id + spec * is;
}

vec3 Uncharted2ToneMapping(vec3 color) {
    float gamma = 2.2;
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 2.;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1. / gamma));
	return color;
}

void main () {
    const float ks = .2;
    const float kd = .8;
    const float ka = .1;
    const float alpha = 8;

    const vec3 ia = vec3(1,0.5,0);
    const vec3 id = vec3(1,0.5,0);
    const vec3 is = vec3(1,1,1);

    vec3 v = normalize(fEye);
    vec3 n = normalize(fNorm);
    vec3 l = lightDir;
    fColor = phong(v, n, l, ia, id, is, ka, kd, ks, alpha);
    fColor = Uncharted2ToneMapping(fColor);
}