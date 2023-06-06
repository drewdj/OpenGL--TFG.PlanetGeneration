#version 430
#extension GL_NV_shadow_samplers_cube : enable


#define PI 3.1415926535897932384626433832795
uniform vec4 lightPos;
uniform vec4 lightColor;

uniform vec3 camPos;

uniform float time;

in vec4 fpos;
in vec4 fnorm;
flat in int fTextType;
in vec4 fcolor;
smooth in float fnoise;
in vec2 ftexCoord;

out vec4 gli_FragColor;


void main()
{
    vec4 colorOcean = vec4(0.0, 0.0, 1.0, 1.0); // Azul
    vec4 colorBeach = vec4(1.0, 1.0, 0.0, 1.0); // Amarillo
    vec4 colorLand = vec4(0.0, 1.0, 0.0, 1.0); // Verde
    vec4 colorMountain = vec4(0.5, 0.5, 0.5, 1.0); // Gris
    vec4 colorPeak = vec4(1.0, 1.0, 1.0, 1.0); // Blanco

    float beachStart = 0.3;
    float landStart = 0.4;
    float mountainStart = 0.6;
    float peakStart = 0.8;

    vec4 baseColor;

    if (fnoise < beachStart) {
        baseColor = colorOcean;
    } else if (fnoise < landStart) {
        baseColor = mix(colorBeach, colorLand, smoothstep(beachStart, landStart, fnoise));
    } else if (fnoise < mountainStart) {
        baseColor = mix(colorLand, colorMountain, smoothstep(landStart, mountainStart, fnoise));
    } else if (fnoise < peakStart) {
        baseColor = mix(colorMountain, colorPeak, smoothstep(mountainStart, peakStart, fnoise));
    } else {
        baseColor = colorPeak;
    }

    // Ajustar luminosidad basada en la altura
    float brightness = mix(1.2, 0.6, fnoise);
    vec4 color = vec4(baseColor.rgb * brightness, baseColor.a);

    float ambient = 0.05;
    
    vec4 normal = normalize(fnorm);
    vec3 viewDir = normalize(camPos - fpos.xyz);
    vec4 lightDir = normalize(lightPos - fpos);
    float diffuse = max(dot(normal, lightDir), 0.0);

    float specularLight = 0.5;
    vec4 reflectDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectDir.xyz), 0.0), 8);
    float specular = specularLight * specAmount;
    
    gli_FragColor = color * lightColor * (diffuse + ambient + specular);


}


