#version 430
#extension GL_NV_shadow_samplers_cube : enable

layout(location=3) uniform samplerCube cubeMap;



in vec4 fpos;


out vec4 gli_FragColor;

void main()
{	
		vec4 textureColor = texture(cubeMap,fpos.xyz);
		
		gli_FragColor = textureColor;
}