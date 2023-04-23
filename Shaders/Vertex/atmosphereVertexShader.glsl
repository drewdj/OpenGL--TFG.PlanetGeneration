#version 430
#extension GL_NV_shadow_samplers_cube : enable

uniform mat4 MVP;
uniform mat4 M;
uniform int textType;


layout(location=0)in vec4 vpos;
layout(location=1)in vec4 vcolor;
layout(location=2)in vec4 vnorm;


out vec4 fpos;
out vec4 fnorm;
flat out int fTextType;

void main() {

	fpos=vpos;	
	fnorm=normalize(inverse(transpose(M))*vnorm);
	vec4 pos = MVP * vpos;	
	
	gl_Position=pos;
}