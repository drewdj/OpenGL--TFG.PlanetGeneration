#version 430
#extension GL_NV_shadow_samplers_cube : enable

layout(location=0) uniform mat4 MVP;
layout(location=1) uniform mat4 M;
layout(location=3) uniform samplerCube cubeMap;
layout(location=4) uniform int textType;


layout(location=0)in vec4 vpos;
layout(location=2)in vec4 vnorm;


out vec4 fpos;


void main() {

	fpos=M*vpos;

	vec4 pos = MVP * vpos;	
		
	//z component always 1
	gl_Position = vec4(pos.x,pos.y,pos.w,pos.w);

}