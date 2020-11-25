float random(vec2 seed) {
    return fract(sin(dot(seed + gl_FragCoord.xy + iTime * 10.,
        vec2(12.9898,78.233)))*
        43758.5453123);
}

vec2 randomS(vec2 seed) {
    float angle = random(seed) * (3.14159 * 2.);
    return vec2(cos(angle), sin(angle));
}

float value(vec2 p) {
    float l = length(p);
    p *= 1./l;
    float v = 2. * p.x * p.y;
    return l > .9 ? v : -v;
}

float distance(vec2 p) {
    return min(abs(length(p) - 1.), abs(length(p) - 0.7));
}

float WoS(vec2 p, vec2 seed) {
    for (int i = 0; i < 30; i++) {
        float d = distance(p);
        if (d < 0.001) return value(p);
        p += d * randomS(seed + p * 10.);
    }
    return 0.;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = (fragCoord.xy / iResolution.xy * 2. - 1.)
			* vec2(iResolution.x / iResolution.y, 1) * 1.5;

    vec3 col = vec3(0);
    
    float v = 0.;
    int N = 100;
    for (int i = 0; i < N; i++) {
   		v += WoS(uv, vec2(i, -i) * 10.);
    }
    v /= float(N);
    col = vec3(+v, -v, -v);
    
    fragColor = vec4(clamp(col, 0., 1.), 1);
}
