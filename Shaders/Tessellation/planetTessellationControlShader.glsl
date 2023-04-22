#version 460 core

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
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

	
    float minTessellation = 1.0;
    float maxTessellation = 10.0;
    float maxDistance = 5.0;

	
if (gl_InvocationID == 0) // Planeta
{
    vec4 center_object_space = (tescontrol_pos[0] + tescontrol_pos[1] + tescontrol_pos[2]) / 3.0;
    vec4 center_eye_space = V * M * center_object_space;

    // Utiliza la componente z en el espacio de la vista para ajustar la teselación
    float distToCamera = abs(center_eye_space.z);

    // Ajustar el nivel de teselación en función de la distancia a la cámara
    float tessellationFactor = 1.0 - clamp(distToCamera / maxDistance, 0.0, 1.0);
    float adjustedTessellation = mix(minTessellation, maxTessellation, tessellationFactor);

    gl_TessLevelOuter[0] = adjustedTessellation;
    gl_TessLevelOuter[1] = adjustedTessellation;
    gl_TessLevelOuter[2] = adjustedTessellation;

    gl_TessLevelInner[0] = adjustedTessellation;


}



    
}


