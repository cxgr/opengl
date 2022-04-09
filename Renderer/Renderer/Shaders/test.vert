 #version 330
 
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex0;

out vec4 vCol;
out vec2 TexCoord0;

uniform mat4 mtxModel;
uniform mat4 mtxProj;
uniform mat4 mtxView;

void main()
{
	gl_Position =  mtxProj * mtxView * mtxModel * vec4(pos, 1.0);
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
	TexCoord0 = tex0;
}