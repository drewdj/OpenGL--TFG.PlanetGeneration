#version 460 core

uniform mat4 MVP;
uniform mat4 M;

layout (triangles, equal_spacing, ccw) in;

in vec4 tescontrol_pos[];
in vec4 tescontrol_norm[];
patch in int tescontrol_TextType;

out vec4 fpos;


void main() {    

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    vec3 pos = vec3(0.0);

    for (int i = 0; i < 3; ++i) {
        pos += tescontrol_pos[i].xyz * gl_TessCoord[i];
    }


    fpos = vec4(pos, 1.0);
    
    gl_Position = MVP * fpos;
}

    




    