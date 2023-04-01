#version 430
#extension GL_NV_shadow_samplers_cube : enable

layout(location=2) uniform vec4 lightPos;
layout (location=6) uniform float radius;
layout(location=9) uniform vec4 lightColor;


in vec4 fpos;
in vec4 fnorm;
flat in int fTextType;
in vec4 fcolor;

out vec4 gli_FragColor;

void main()
{	
	float radius = distance(fpos, vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
    vec4 fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        if(0.9 < radius && radius < 1.01)
        {
            fragColor = vec4(0.0f,0.0f,0.5-(1-radius)*7,1.0f);
        }
        if(1.01 < radius && radius < 1.025)
        {
            fragColor = vec4(0.0f,0.5-(radius-1)*8,0.0f,1.0f);
        }
        if(1.025 < radius && radius < 1.05)
        {
            fragColor = vec4(0.5-(radius-1)*2,0.5-(radius-1)*6,0.1f,1.0f);
        }
        if(1.05 < radius)// && radius < 1.1)
        {
            fragColor = vec4(0.9-(radius-1)*2,0.95-(radius-1),1.0f,1.0f);
        }

        //vec4 normal = normalize(fpos);
        //vec4 lightDir = normalize(lightPos - fpos);        
        //float diffuse = max(dot(normal, lightDir), 0.0);         
        //gli_FragColor = fragColor * lightColor * diffuse;
        
        gli_FragColor = fragColor;
}