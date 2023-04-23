#version 430

uniform vec4 lightPos;

in vec4 fpos;
in vec4 fnorm;

out vec4 gli_FragColor;

const float atmosphereThickness = 0.025;
const float scatterStrength = 8.0;
const vec3 skyColor = vec3(0.5, 0.7, 1.0);

void main()
{   
    vec3 normal = normalize(fnorm.xyz);
    vec3 viewDirection = normalize(fpos.xyz - vec3(0.0, 0.0, 0.0));
    float cosTheta = dot(viewDirection, normal);
    
    float atmosphereDensity = 1.0 - smoothstep(0.0, atmosphereThickness, cosTheta);
    vec3 scatterColor = skyColor * pow(atmosphereDensity, scatterStrength);

    vec3 lightDir = normalize(lightPos.xyz - fpos.xyz);
    
    float lightIntensity = max(dot(normal, -lightDir), 0.0);
    vec3 lightColor = scatterColor * lightIntensity;

    // Set alpha value based on the atmosphereDensity
    float alpha = atmosphereDensity;

    // Depth-based blending
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    gli_FragColor = vec4(lightColor, alpha * depth);
}
