 #version 330
 
layout(location = 0) in vec3 pos;

out vec4 vCol;

uniform mat4 mtxModel;
uniform mat4 mtxProj;
uniform mat4 mtxView;

void main()
{
	gl_Position =  mtxProj * mtxView * mtxModel * vec4(pos, 1.0);
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}