#version 460 core

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec4 camPos;
uniform int tessellation;
uniform float radius;



layout (vertices = 3) out;

in vec4 fpos[];
in vec4 fnorm[];
flat in int fTextType[];



out vec4 tescontrol_pos[];
out vec4 tescontrol_norm[];
patch out int tescontrol_TextType;



void main(){

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	tescontrol_pos[gl_InvocationID] = fpos[gl_InvocationID];
	tescontrol_norm[gl_InvocationID] = fnorm[gl_InvocationID];
	tescontrol_TextType = fTextType[gl_InvocationID];

	
    float minTessellation = 1.0;
    float maxTessellation = 10.0;
    float maxDistance = 0.5;

	
if (gl_InvocationID == 0) // Planeta
{
    
	vec4 triangleCenter = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
	
	float distanceToCamera = distance(triangleCenter, camPos) - 1;

	
	gl_TessLevelOuter[0] = distanceToCamera;
	gl_TessLevelOuter[1] = distanceToCamera;
	gl_TessLevelOuter[2] = distanceToCamera;
	gl_TessLevelInner[0] = distanceToCamera;

}




    
}


