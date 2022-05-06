#version 330

layout(location = 0) in vec3 pos;

uniform mat4 mtxModel;
uniform mat4 mtxDirLightTransform; //p * v

void main()
{
	gl_Position = mtxDirLightTransform * mtxModel * vec4(pos, 1.f);
}