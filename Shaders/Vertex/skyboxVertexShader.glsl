#version 430
#extension GL_NV_shadow_samplers_cube : enable

uniform mat4 MVP;
uniform mat4 M;

layout(location=0)in vec4 vpos;


out vec4 fpos;


void main() {

	fpos=M*vpos;

	vec4 pos = MVP * vpos;	
		
	//z component always 1
	gl_Position = vec4(pos.x,pos.y,pos.w,pos.w);

}