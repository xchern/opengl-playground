#version 330

uniform vec3 lightDir;

in vec3 fNorm;
in vec3 fEye;

out vec3 fColor; 

vec3 brdf(vec3 v, vec3 n, vec3 l);

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
    vec3 v = normalize(fEye);
    vec3 n = normalize(fNorm);
    vec3 l = lightDir;
    fColor = brdf(v, n, l);
    fColor = Uncharted2ToneMapping(fColor);
    //fColor = vec3(gl_FragCoord.z);
}