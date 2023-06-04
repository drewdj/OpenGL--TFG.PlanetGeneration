#version 430
#extension GL_NV_shadow_samplers_cube : enable


#define PI 3.1415926535897932384626433832795
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

    float repeatFactor = 10;
    // Convertir la posición a coordenadas polares
    float lon = atan(fpos.z, fpos.x);
    float lat = asin(fpos.y);
    
    // Normalizar a [0, 1]
    float u = lon / (2.0 * PI) + 0.5;
    float v = lat / PI + 0.5;

    // Repite la textura
    u *= repeatFactor;
    v *= repeatFactor;

    gli_FragColor = texture(u_Textures[0], vec2(u, v));


}