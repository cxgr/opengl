#version 330

layout(location = 0) in vec3 pos;

out vec3 TexCoords;

uniform mat4 mtxProj;
uniform mat4 mtxView;

void main()
{
	TexCoords = pos;
	gl_Position = mtxProj * mtxView * vec4(pos, 1.0);
}