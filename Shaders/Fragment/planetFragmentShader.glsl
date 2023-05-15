#version 430
#extension GL_NV_shadow_samplers_cube : enable



uniform vec4 lightPos;
uniform vec4 lightColor;

uniform vec3 camPos;

uniform vec4 waterColor;
uniform vec4 landColor;
uniform vec4 mountainColor;

uniform float waterLevel;
uniform float landLevel;
uniform float mountainLevel;
uniform sampler2D u_Textures[3]; // Para 3 texturas: arena, hierba y roca



in vec4 fpos;
in vec4 fnorm;
flat in int fTextType;
in vec4 fcolor;
flat in float fnoise;
in vec2 ftexCoord;

out vec4 gli_FragColor;

void main()
{	


vec3 sandColor = texture(u_Textures[0], ftexCoord).rgb;
vec3 grassColor = texture(u_Textures[1], ftexCoord).rgb;
vec3 rockColor = texture(u_Textures[2], ftexCoord).rgb;

// Establece límites de altura para cada textura
float sandThreshold = 0.3;
float grassThreshold = 0.6;

vec3 finalColor;
if (fnoise < sandThreshold) {
    finalColor = sandColor;
} else if (fnoise < grassThreshold) {
    finalColor = mix(sandColor, grassColor, smoothstep(sandThreshold, grassThreshold, fnoise));
} else {
    finalColor = mix(grassColor, rockColor, smoothstep(grassThreshold, 1.0, fnoise));
}

finalColor = texture(u_Textures[0], ftexCoord).rgb;

// Asigna el color final al fragmento
gli_FragColor = vec4(finalColor, 1.0);


}