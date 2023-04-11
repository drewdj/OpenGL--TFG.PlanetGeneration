#version 430
#extension GL_NV_shadow_samplers_cube : enable

uniform samplerCube textureUnit;



in vec4 fpos;


out vec4 gli_FragColor;

void main()
{	
		vec4 textureColor = texture(textureUnit,fpos.xyz);
		
		gli_FragColor = textureColor;
}