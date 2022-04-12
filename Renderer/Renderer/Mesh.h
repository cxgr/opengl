#pragma once
#include <glew.h>
#include <glm/vec3.hpp>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void CreateMesh(GLfloat* verts, size_t numVerts, unsigned* indices, size_t numIndices);
	void RenderMesh();
	void ClearMesh();

	static void CalculateAvgNormals(unsigned* indices, unsigned idxCount, GLfloat* verts, unsigned vertCount,
	                                unsigned vLength, unsigned nrmOffset);

private:
	GLuint VAO, VBO, IBO;
	size_t indexCount;
};

