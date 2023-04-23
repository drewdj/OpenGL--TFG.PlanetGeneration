#version 430

uniform mat4 MVP;
uniform mat4 M;
uniform float radius;

layout(location=0) in vec4 vpos;
layout(location=1) in vec4 vcolor;
layout(location=2) in vec4 vnorm;

out vec4 fpos;
out vec4 fnorm;

void main() {
    vec4 displacedPos = vec4(vpos.xyz * radius, 1.0);

    fpos = displacedPos;
	fnorm=normalize(inverse(transpose(M))*vnorm);
    gl_Position = MVP * displacedPos;
}
