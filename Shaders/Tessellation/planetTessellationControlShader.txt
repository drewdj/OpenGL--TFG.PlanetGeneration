#version 460 core

layout(location=0) uniform mat4 MVP;
layout(location=1) uniform mat4 M;
layout(location=7) uniform mat4 V;
layout (location=5) uniform vec3 camPos;


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

	
if (gl_InvocationID == 0) //Planeta
    {

		//TODO cambiar a lados no centro
		
		//float angle = acos(dot(camPos, fpos[0].xyz) / (length(camPos) * length(fpos[0].xyz)));
		
		vec4 center = (fpos[0] + fpos[1] + fpos[2]) / 3.0;
		
		center = V * center;
		
		float dist = -center.z;
			


		gl_TessLevelOuter[0] = 64; 
        gl_TessLevelOuter[1] = 64; 
        gl_TessLevelOuter[2] = 64; 
        
        gl_TessLevelInner[0] = 64;
    }


    
}


