#pragma once
#include <glew.h>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void CreateMesh(GLfloat* verts, unsigned int numVerts, unsigned int* indices, unsigned int numIndices);
	void RenderMesh();
	void ClearMesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

