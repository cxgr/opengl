#pragma once
#include <glew.h>
#include <glm/vec3.hpp>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void CreateMesh(GLfloat* verts, unsigned int numVerts, unsigned int* indices, unsigned int numIndices);
	void RenderMesh();
	void ClearMesh();

	static void CalculateAvgNormals(unsigned* indices, unsigned idxCount, GLfloat* verts, unsigned vertCount,
	                                unsigned vLength, unsigned nrmOffset);

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

