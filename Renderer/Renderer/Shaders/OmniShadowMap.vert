#version 330

layout (location = 0) in vec3 pos;

uniform mat4 mtxModel;

void main()
{
	gl_Position = mtxModel * vec4(pos, 1.f);
}