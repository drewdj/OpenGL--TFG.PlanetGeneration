#version 430

uniform vec4 lightPos;
uniform vec4 camPos;

uniform float planetRadius;
uniform float atmosphereRadius;
uniform vec3 rayleighScattering;
uniform float mieScattering;
uniform vec2 hesightScale;
uniform float refraction;

const float M_PI = 3.1415926535897932384626433832795;

in vec4 fpos;
in vec4 fnorm;

out vec4 gli_FragColor;

bool intersectSphere(vec3 rayOrigin, vec3 rayDir, float radius, out float t0, out float t1) {
    vec3 oc = rayOrigin;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(oc, rayDir);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return false;
    } else {
        t0 = (-b - sqrt(discriminant)) / (2.0 * a);
        t1 = (-b + sqrt(discriminant)) / (2.0 * a);
        return true;
    }
}

vec2 particleDensity(float height) {
    float r = height / hesightScale.x;
    float m = height / hesightScale.y;
    return vec2(exp(-r), exp(-m));
}

vec3 pointOnRay(vec3 origin, vec3 direction, float t) {
    return origin + direction * t;
}

float heightAtPoint(vec3 point) {
    return length(point) - planetRadius;
}

vec4 scatteringCoefficient(vec2 densities) {
    vec3 rayleigh = densities.x * rayleighScattering;
    float mie = densities.y * mieScattering;
    return vec4(rayleigh, mie);
}

float henyeyGreensteinPhase(float cosTheta, float g) {
    float g2 = g * g;
    float denominator = 1.0 + g2 - 2.0 * g * cosTheta;
    return (1.0 - g2) / (4.0 * M_PI * sqrt(denominator * denominator * denominator));
}


void main() {

    vec3 lightDir = normalize(lightPos.xyz - fpos.xyz);
    vec3 viewDir = normalize(camPos.xyz - fpos.xyz);

    float t0, t1;
    bool intersects = intersectSphere(camPos.xyz, lightDir, atmosphereRadius, t0, t1);

    vec3 point = pointOnRay(camPos.xyz, lightDir, 0.5); //ultimo valor t varia entre 0 y 1
    float height = heightAtPoint(point);
    vec2 densities = particleDensity(height);

    vec4 scatteringCoeff = scatteringCoefficient(densities);

    float cosTheta = dot(lightDir, viewDir);

    float rayleighPhase = henyeyGreensteinPhase(cosTheta, 0.0);
    float miePhase = henyeyGreensteinPhase(cosTheta, refraction);

    // Calcular el factor de atenuación basado en la altura
    float attenuation = 1.0 - smoothstep(planetRadius, atmosphereRadius, length(point));

    // Multiplicar las densidades de Rayleigh y Mie por el factor de atenuación
    vec3 scaledDensities = (rayleighScattering * densities.x + mieScattering * densities.y) * attenuation;

    // Usar la suma de las densidades escaladas como el valor de alpha
    float alpha = min(1.0, max(0.0, dot(scaledDensities, vec3(1.0))));

    vec3 finalColor = scatteringCoeff.rgb * (rayleighPhase + miePhase * scatteringCoeff.a);
    gli_FragColor = vec4(finalColor, alpha);
}

