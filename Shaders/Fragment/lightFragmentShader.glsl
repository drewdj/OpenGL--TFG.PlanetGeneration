#version 430

layout(location=9) uniform vec4 lightColor;


out vec4 gli_FragColor;

void main()
{			
		gli_FragColor = lightColor;
}