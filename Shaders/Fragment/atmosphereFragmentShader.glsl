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


void main() {


}

