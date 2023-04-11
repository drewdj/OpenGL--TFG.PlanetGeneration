#version 460 core

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 camPos;
uniform int tessellation;


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
			


		gl_TessLevelOuter[0] = tessellation; 
        gl_TessLevelOuter[1] = tessellation; 
        gl_TessLevelOuter[2] = tessellation; 
        
        gl_TessLevelInner[0] = tessellation;
    }


    
}


