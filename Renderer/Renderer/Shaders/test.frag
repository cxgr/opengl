#version 330

in vec4 vCol;
in vec2 TexCoord0;

out vec4 col;

struct DirectionalLight
{
	vec3 color;
	float ambientIntensity;
};

uniform sampler2D sampler0;
uniform DirectionalLight dirLight;

void main()
{
	vec4 ambColor = vec4(dirLight.color, 1.f) * dirLight.ambientIntensity;
	
	col = texture(sampler0, TexCoord0) * ambColor;
}