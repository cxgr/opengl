#version 330

in vec3 TexCoords;

out vec4 col;

uniform samplerCube skybox;

void main()
{
	col = texture(skybox, TexCoords);
}